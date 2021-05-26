#include "../header/DistributedSKSolver.h"

using namespace std;

namespace DistributedSKSolver
{
    int solveMaxDepthSudoku(queue<int *> &pool, int depth, int maxDepth, int heuristic, int *steps, int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
    {
        (*steps)++;

        bool isSolved = isSolution(state, rows, cols);

        if (logger != nullptr)
        {
            logger->info("{}\t{}\t{}\t{:.4}\t{}", 0, *steps, depth, stopwatch, isSolved);
        }

        if (isSolved)
        {
            return 1;
        }
        else if (depth >= maxDepth)
        {
            int *stateToSave = new int[rows * cols];
            copy(state, state + (rows * cols), stateToSave);
            pool.push(stateToSave);
        }
        else
        {
            pair<int, int> nextPos;

            switch (heuristic)
            {
            case NORMAL:
                nextPos = findNextZero(state, rows, cols);
                break;
            case HEURISTIC1:
                nextPos = findNextZeroByBenefit(state, regionX, regionY, rows, cols, blocks);
                break;
            case HEURISTIC2:
                nextPos = findNextZeroBySum(state, regionX, regionY, rows, cols, blocks);
                break;
            case HEURISTIC3:
                nextPos = findNextZeroBy45Rule(state, regionX, regionY, rows, cols, blocks);
                break;
            default:
                break;
            }

            for (int &alternative : getAlternatives(rows, cols))
            {
                if (isSafe(state, rows, cols, regionX, regionY, blocks, nextPos.first, nextPos.second, alternative))
                {
                    moveForward(state, rows, cols, nextPos.first, nextPos.second, alternative);

                    int isSolved = solveMaxDepthSudoku(pool, depth + 1, maxDepth, heuristic, steps, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);
                    if (isSolved == 1)
                    {
                        return isSolved;
                    }

                    moveBackward(state, rows, cols, nextPos.first, nextPos.second);
                }
            }
        }

        return 0;
    }

    int master(int heuristic, int rank, int size, int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
    {
        spdlog::info("------------- Starting master -------------");

        bool *busy = new bool[size];
        busy[0] = true;
        for (size_t i = 1; i < size; i++)
        {
            busy[i] = false;
        }
        int idle = size - 1;
        int tmp = 0;

        spdlog::info("Master --> Num. of slaves: {}", idle);

        MPI_Send(&tmp, 1, MPI_INT, 1, PBM_TAG, MPI_COMM_WORLD);
        MPI_Send(state, rows * cols, MPI_INT, 1, PBM_TAG, MPI_COMM_WORLD);
        busy[1] = true;
        idle--;

        int isSolved = 0;
        MPI_Status status;
        while (idle < size - 1)
        {
            //spdlog::info("M: waiting {}", idle);
            MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            switch (status.MPI_TAG)
            {
            case SOLVE_TAG:
                MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

                isSolved = 1;

                spdlog::info("Master --> Received solution from {}", status.MPI_SOURCE);
                break;

            case BnB_TAG:
                int requestedSlaves;
                MPI_Recv(&requestedSlaves, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

                if (isSolved != 1)
                {
                    int grantedSlaves = (requestedSlaves <= idle) ? requestedSlaves : idle;
                    MPI_Send(&grantedSlaves, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD);

                    int *slaves = new int[grantedSlaves];
                    int remainingSlaves = grantedSlaves;
                    for (int i = 1; i < size && remainingSlaves > 0; i++)
                    {
                        if (busy[i] == false)
                        {
                            busy[i] = true;
                            idle--;

                            slaves[grantedSlaves - remainingSlaves] = i;
                            remainingSlaves--;
                        }
                    }

                    MPI_Send(slaves, grantedSlaves, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD);
                    delete slaves;
                }
                else
                {
                    MPI_Send(&tmp, 1, MPI_INT, status.MPI_SOURCE, DONE_TAG, MPI_COMM_WORLD);
                }

                break;

            case IDLE_TAG:
                busy[status.MPI_SOURCE] = false;
                idle++;
                break;

            default:
                break;
            }
        }

        spdlog::info("Master --> Finishing slaves...");

        int i = 1;
        while (i < size)
        {
            MPI_Send(&tmp, 1, MPI_INT, i, END_TAG, MPI_COMM_WORLD);
            i++;
        }

        delete busy;

        spdlog::info("------------- Finishing master -------------");
        return isSolved;
    }

    void slave(int heuristic, int rank, int size, int maxDepth, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
    {
        spdlog::info("------------- Starting slave {} -------------", rank);

        int *state = new int[rows * cols];
        queue<int *> pool;
        int steps;
        int isSolved = 0;
        int neededSlaves;
        int grantedSlaves;
        int *slaves;

        MPI_Status status;
        bool active = true;
        int tmp = 0;
        while (active)
        {
            MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            switch (status.MPI_TAG)
            {
            case END_TAG:
                active = false;
                break;

            case PBM_TAG:
                MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

                pool.push(state);
                steps = 0;

                while (!pool.empty() && isSolved != 1)
                {
                    int *state = pool.front();
                    pool.pop();
                    isSolved = solveMaxDepthSudoku(pool, 0, maxDepth, heuristic, &steps, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);

                    if (isSolved == 1)
                    {
                        MPI_Send(&tmp, 0, MPI_INT, 0, SOLVE_TAG, MPI_COMM_WORLD);
                        MPI_Send(state, rows * cols, MPI_INT, 0, SOLVE_TAG, MPI_COMM_WORLD);
                        break;
                    }

                    neededSlaves = pool.size();

                    MPI_Send(&tmp, 1, MPI_INT, 0, BnB_TAG, MPI_COMM_WORLD);
                    MPI_Send(&neededSlaves, 1, MPI_INT, 0, BnB_TAG, MPI_COMM_WORLD);

                    MPI_Recv(&grantedSlaves, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    if (status.MPI_TAG == BnB_TAG)
                    {
                        slaves = new int[grantedSlaves];
                        MPI_Recv(slaves, grantedSlaves, MPI_INT, 0, BnB_TAG, MPI_COMM_WORLD, &status);

                        for (size_t i = 0; i < grantedSlaves; i++)
                        {
                            int *stateToSend = pool.front();
                            MPI_Send(&tmp, 1, MPI_INT, slaves[i], PBM_TAG, MPI_COMM_WORLD);
                            MPI_Send(stateToSend, rows * cols, MPI_INT, slaves[i], PBM_TAG, MPI_COMM_WORLD);
                            delete stateToSend;
                            pool.pop();
                        }

                        delete slaves;
                    }
                    else
                    {
                        isSolved = 1;
                    }
                }
                break;

            default:
                break;
            }

            MPI_Send(&tmp, 0, MPI_INT, 0, IDLE_TAG, MPI_COMM_WORLD);
        }

        spdlog::info("------------- Finishing slave {} -------------", rank);
    }

    int solveSudoku(int rank, int size, int heuristic, double initialMaxDepth, int *steps, int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
    {
        int isSolved = 0;
        if (rank == 0)
        {
            int maxDepth = (int)ceil(initialMaxDepth * countZeros(state, rows, cols));
            isSolved = master(heuristic, rank, size, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);
        }
        else
        {
            int maxDepth = (int)ceil(initialMaxDepth * countZeros(state, rows, cols));
            slave(heuristic, rank, size, maxDepth, rows, cols, regionX, regionY, blocks, stopwatch, logger);
        }

        return isSolved;
    }
}