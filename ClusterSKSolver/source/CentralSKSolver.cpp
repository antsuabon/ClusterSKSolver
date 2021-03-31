#include "../header/CentralSKSolver.h"

using namespace std;

int solveFullSudoku(int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
{
	(*steps)++;

	if (isSolution(state, rows, cols))
	{
		return 0;
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

				int isSolved = solveFullSudoku(heuristic, steps, state, rows, cols, regionX, regionY);
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
	queue<int*> taskPook;

	int steps = 0;
	solveMaxDepthSudoku(taskPook, 0, maxDepth, heuristic, &steps, state, rows, cols, regionX, regionY);

	spdlog::info("------------- Starting master -------------");

	spdlog::info("Master -> {} tasks left!", taskPook.size());

	int activeWorkers;
	for (activeWorkers = 0; activeWorkers < size - 1; activeWorkers++)
	{
		int* stateToSend = taskPook.front();
		MPI_Send(stateToSend, rows * cols, MPI_INT, activeWorkers + 1, 0, MPI_COMM_WORLD);
		delete stateToSend;
		taskPook.pop();
	}

	int isSolved = 1;
	MPI_Status status;
	while (!taskPook.empty() && isSolved != 0)
	{
		MPI_Recv(&isSolved, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		if (isSolved == 0)
		{
			MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
			spdlog::info("Master -> Slave {} found a solution!", status.MPI_SOURCE);
		}

		MPI_Send(&isSolved, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
		if (isSolved != 0)
		{
			int* stateToSend = taskPook.front();
			MPI_Send(stateToSend, rows * cols, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			delete stateToSend;
			taskPook.pop();
		}
		else
		{
			activeWorkers--;
		}
		 

		spdlog::info("Master -> {} tasks left!", taskPook.size());
	}

	while (activeWorkers > 0)
	{
		MPI_Recv(&isSolved, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		if (isSolved == 0)
		{
			MPI_Recv(state, rows * cols, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
			spdlog::info("Master -> Slave {} found a solution!", status.MPI_SOURCE);
		}

		int stop = 0;
		MPI_Send(&stop, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
		activeWorkers--;
	}
	
	spdlog::info("------------- Finishing master -------------");

	return isSolved;
}

void slave(int heuristic, int rank, int size, int rows, int cols, int regionX, int regionY)
{
	int* state = new int[rows * cols];

	spdlog::info("------------- Starting slave {} -------------", rank);

	int isSolved = 1;
	MPI_Status status;
	while(isSolved != 0)
	{
		MPI_Recv(state, rows * cols, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		spdlog::info("Slave {} -> Received task!", rank);

		int steps = 0;
		isSolved = solveFullSudoku(heuristic, &steps, state, rows, cols, regionX, regionY);

		MPI_Send(&isSolved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		if (isSolved == 0)
		{
			spdlog::info("Slave {} -> Found solution!", rank);
			MPI_Send(state, rows * cols, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}

		MPI_Recv(&isSolved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);	
	}

	spdlog::info("------------- Finishing slave {} -------------", rank);

	delete state;
}

int solveSudoku(int rank, int size, int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
{
	if (rank == 0)
	{
		int isSolved = master(heuristic, rank, size, 5, state, rows, cols, regionX, regionY);

		return isSolved;
	}
	else
	{
		slave(heuristic, rank, size, rows, cols, regionX, regionY);
	}

	return 1;
}