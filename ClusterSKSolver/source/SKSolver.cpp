#include "../header/SKSolver.h"

using namespace std;

namespace SKSolver
{
	int solveSudoku(int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY)
	{
		(*steps)++;

		bool isSolved = isSolution(state, rows, cols);

		//spdlog::get("basic_logger")->info("{} {} {} {}", 0, *steps, isSolved, 0);

		if (isSolved)
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

					int isSolved = solveSudoku(heuristic, steps, state, rows, cols, regionX, regionY);
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
}