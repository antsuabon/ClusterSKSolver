#include "../header/SKSolver.h"

using namespace std;

int solveSudoku(int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
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

		for each (int alternative in getAlternatives(rows, cols))
		{
			if (isSafe(state, rows, cols, regionX, regionY, nextPos.first, nextPos.second, alternative))
			{
				moveForward(state, rows, cols, nextPos.first, nextPos.second, alternative);

				solveSudoku(heuristic, steps, state, rows, cols, regionX, regionY);
				if (isSolution(state, rows, cols))
				{
					return 0;
				}

				moveBackward(state, rows, cols, nextPos.first, nextPos.second);
			}
		}
	}

	return 1;
}