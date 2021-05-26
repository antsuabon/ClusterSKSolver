#include "../header/CentralSKSolver.h"

using namespace std;

namespace CentralSKSolver
{
	int solveFullSudoku(int heuristic, int *steps, int *depth, int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
	{
		(*steps)++;

		bool isSolved = isSolution(state, rows, cols);

		if (logger != nullptr)
		{
			logger->info("{}\t{}\t{}\t{:.4}\t{}", 0, *steps, *depth, stopwatch, isSolved);
		}

		if (isSolved)
		{
			return 1;
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

					int isSolved = solveFullSudoku(heuristic, steps, depth, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);
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

	// ####################################################################################################################################################
	// ####################################################################################################################################################

	// ####################################################################################################################################################
	// ####################################################################################################################################################

	int master(int heuristic, int rank, int size, int maxDepth, int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
	{
		queue<int *> taskPool;

		int steps = 0;
		solveMaxDepthSudoku(taskPool, 0, maxDepth, heuristic, &steps, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);

		spdlog::info("------------- Starting master -------------");

		spdlog::info("Master -> {} tasks left!", taskPool.size());

		int activeWorkers;
		for (activeWorkers = 0; activeWorkers < size - 1; activeWorkers++)
		{
			int *stateToSend = taskPool.front();
			MPI_Send(stateToSend, rows * cols, MPI_INT, activeWorkers + 1, 0, MPI_COMM_WORLD);
			delete stateToSend;
			taskPool.pop();
		}

		int isSolved = 0;
		int answer = 1;
		MPI_Status status;

		while (!taskPool.empty() && isSolved != 1)
		{

			MPI_Recv(&answer, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			if (answer == 1)
			{
				MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
				spdlog::info("Master -> Slave {} found a solution!", status.MPI_SOURCE);
				isSolved = answer;
			}

			MPI_Send(&isSolved, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			if (isSolved != 1)
			{
				int *stateToSend = taskPool.front();
				MPI_Send(stateToSend, rows * cols, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				delete stateToSend;
				taskPool.pop();
			}
			else
			{
				activeWorkers--;
			}

			spdlog::info("Master -> {} tasks left!", taskPool.size());
		}

		while (activeWorkers > 0)
		{

			MPI_Recv(&answer, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			if (answer == 1)
			{

				MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
				spdlog::info("Master -> Slave {} found a solution!", status.MPI_SOURCE);

				isSolved = answer;
			}

			int stop = 1;
			MPI_Send(&stop, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			activeWorkers--;
		}

		spdlog::info("------------- Finishing master -------------");

		return isSolved;
	}

	void slave(int heuristic, int rank, int size, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
	{
		int *state = new int[rows * cols];

		spdlog::info("------------- Starting slave {} -------------", rank);

		int isSolved = 0;
		MPI_Status status;
		while (isSolved != 1)
		{
			MPI_Recv(state, rows * cols, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			spdlog::info("Slave {} -> Received task!", rank);

			int steps = 0;
			int depth = 0;
			isSolved = solveFullSudoku(heuristic, &steps, &depth, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);

			MPI_Send(&isSolved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

			if (isSolved == 1)
			{
				spdlog::info("Slave {} -> Found solution!", rank);
				MPI_Send(state, rows * cols, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}

			MPI_Recv(&isSolved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		}

		spdlog::info("------------- Finishing slave {} -------------", rank);

		delete state;
	}

	// ####################################################################################################################################################

	/**
	 * 
	 */
	int solveSudoku(int rank, int size, int heuristic, double initialMaxDepth, int *steps, int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
	{
		int isSolved = 0;

		if (rank == 0)
		{
			int maxDepth = (int)ceil(initialMaxDepth * countZeros(state, rows, cols));

			spdlog::info("Max. depth: {}", maxDepth);
			isSolved = master(heuristic, rank, size, maxDepth, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);

			return isSolved;
		}
		else
		{
			slave(heuristic, rank, size, rows, cols, regionX, regionY, blocks, stopwatch, logger);
		}

		return isSolved;
	}
}