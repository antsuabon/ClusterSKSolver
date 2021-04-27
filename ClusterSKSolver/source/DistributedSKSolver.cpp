#include "../header/DistributedSKSolver.h"

using namespace std;

namespace DistributedSKSolver
{
    int solveMaxDepthSudoku(queue<int*> &pool, int depth, int maxDepth, int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
	{
		(*steps)++;

		if (isSolution(state, rows, cols))
		{
			return 0;
		}
		else if (depth >= maxDepth)
		{
			int* stateToSave = new int[rows * cols];
			copy(state, state + (rows*cols), stateToSave);
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
				nextPos = findNextZeroByBenefit(state, regionX, regionY, rows, cols);
				break;
			default:
				break;
			}

			for (int &alternative: getAlternatives(rows, cols))
			{
				if (isSafe(state, rows, cols, regionX, regionY, nextPos.first, nextPos.second, alternative))
				{
					moveForward(state, rows, cols, nextPos.first, nextPos.second, alternative);

					int isSolved = solveMaxDepthSudoku(pool, depth + 1, maxDepth, heuristic, steps, state, rows, cols, regionX, regionY);
					if (isSolved == 0)
					{
						return isSolved;
					}

					moveBackward(state, rows, cols, nextPos.first, nextPos.second);
				}
			}
		}

		return 1;
	}

    int master(int heuristic, int rank, int size, int maxDepth, int* state, int rows, int cols, int regionX, int regionY)
    {
        spdlog::info("------------- Starting master -------------");

        bool* busy = new bool[size];
        busy[0] = true;
        for (size_t i = 1; i < size; i++)
        {
            busy[i] = false;
        }
        int idle = size - 1;

        spdlog::info("Master --> Num. of slaves: {}", idle);

        MPI_Send(nullptr, 0, MPI_DATATYPE_NULL, 1, PBM_TAG, MPI_COMM_WORLD);
        MPI_Send(state, rows * cols, MPI_INT, 1, PBM_TAG, MPI_COMM_WORLD);
        busy[1] = true;
        idle--;

        int isSolved = 1;
        MPI_Status status;
        while (idle < size - 1)
        {
            MPI_Recv(nullptr, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            
            switch (status.MPI_TAG)
            {
            case SOLVE_TAG:
                MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
                
                isSolved = 0;
                
                spdlog::info("Master --> Received solution from {}", status.MPI_SOURCE);
                break;

            case BnB_TAG:
                if (isSolved != 0)
                {
                    int requestedSlaves;
                    MPI_Recv(&requestedSlaves, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
                    int grantedSlaves = (requestedSlaves <= idle) ? requestedSlaves : idle;
                    MPI_Send(&grantedSlaves, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD);
                    
                    int* slaves = new int[grantedSlaves];
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
                    MPI_Send(nullptr, 0, MPI_DATATYPE_NULL, status.MPI_SOURCE, DONE_TAG, MPI_COMM_WORLD);
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
            MPI_Send(nullptr, 0, MPI_DATATYPE_NULL, i, END_TAG, MPI_COMM_WORLD);
            i++;
        }

        delete busy;

        spdlog::info("------------- Finishing master -------------");
        return 1;
    }

    void slave(int heuristic, int rank, int size, int rows, int cols, int regionX, int regionY)
    {
        spdlog::info("------------- Starting slave {} -------------", rank);

        int* state = new int[rows * cols];
        queue<int*> pool;
        int steps;
        int isSolved;
        int neededSlaves;
        int grantedSlaves;
        int* slaves;

        MPI_Status status;
        bool active = true;
        while (active)
        {
            MPI_Recv(nullptr, 0, MPI_DATATYPE_NULL, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            switch (status.MPI_TAG)
            {
            case END_TAG:
                active = false;
                break;

            case PBM_TAG:
                MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

                pool.push(state);
                steps = 0;

                while (pool.size() > 0)
                {
                    int* state = pool.front();
                    pool.pop();
                    isSolved = solveMaxDepthSudoku(pool, 0, 10, 1, &steps, state, rows, cols, regionX, regionY);
                
                    if (isSolved == 0)
                    {
                        MPI_Send(nullptr, 0, MPI_DATATYPE_NULL, 0, SOLVE_TAG, MPI_COMM_WORLD);
                        MPI_Send(state, rows * cols, MPI_INT, 0, SOLVE_TAG, MPI_COMM_WORLD);
                        break;
                    }

                    
                    neededSlaves = pool.size();
                    
                    MPI_Send(nullptr, 0, MPI_DATATYPE_NULL, 0, BnB_TAG, MPI_COMM_WORLD);
                    MPI_Send(&neededSlaves, 1, MPI_INT, 0, BnB_TAG, MPI_COMM_WORLD);


                    MPI_Recv(&grantedSlaves, 1, MPI_INT, 0, BnB_TAG, MPI_COMM_WORLD, &status);


                    slaves = new int[grantedSlaves];
                    MPI_Recv(slaves, grantedSlaves, MPI_INT, 0, BnB_TAG, MPI_COMM_WORLD, &status);

                    for (size_t i = 0; i < grantedSlaves; i++)
                    {
                        int* stateToSend = pool.front();
                        MPI_Send(nullptr, 0, MPI_DATATYPE_NULL, slaves[i], PBM_TAG, MPI_COMM_WORLD);
                        MPI_Send(stateToSend, rows * cols, MPI_INT, slaves[i], PBM_TAG, MPI_COMM_WORLD);
                        delete stateToSend;
                        pool.pop();
                    }


                    delete slaves;

                }
                break;

            default:
                break;
            }

            MPI_Send(nullptr, 0, MPI_DATATYPE_NULL, 0, IDLE_TAG, MPI_COMM_WORLD);

        }
        
        spdlog::info("------------- Finishing slave {} -------------", rank);
    }

    int solveSudoku(int rank, int size, int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
    {
        int isSolved = 1;
        if (rank == 0)
        {
            isSolved = master(heuristic, rank, size, 5, state, rows, cols, regionX, regionY);
        }
        else
        {
            slave(heuristic, rank, size, rows, cols, regionX, regionY);
        }
        
        return isSolved;
    }
}