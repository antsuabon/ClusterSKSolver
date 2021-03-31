#include "../header/CentralSKSolver.h"

using namespace std;

int master(int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
{
	int activeWorkers;

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

		for each (int alternative in getAlternatives(rows, cols))
		{
			if (isSafe(state, rows, cols, regionX, regionY, nextPos.first, nextPos.second, alternative))
			{
				moveForward(state, rows, cols, nextPos.first, nextPos.second, alternative);

				if (true)
				{
					MPI_Send(state, 81, MPI_INT, 1, 0, MPI_COMM_WORLD);
				}

				moveBackward(state, rows, cols, nextPos.first, nextPos.second);
			}
		}


		MPI_Status status;
		MPI_Probe(1, 0, MPI_COMM_WORLD, &status);
		
		if (status.internal == MPI_SUCCESS)
		{
			MPI_Recv(state, 81, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			if (isSolution(state, rows, cols))
			{
				state = state;
				return 0;
			}
		}
	}

	return 1;

}

void slave()
{

}

int solveSudoku(int rank, int size, int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
{



	

	return 1;
}