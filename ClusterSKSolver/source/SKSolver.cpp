#include "../header/SKSolver.h"

using namespace std;

namespace SKSolver
{
	int solveSudoku(int heuristic, int *steps, int *depth, int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
	{
		(*steps)++;

		bool isSolved = isSolution(state, rows, cols);

		if (logger != nullptr)
		{
			logger->info("{}\t{}\t{}\t{:.4}\t{}\t{}", 0, *steps, *depth, stopwatch, isSolved, printStateLog(state, rows, cols));
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
					(*depth)++;

					int isSolved = solveSudoku(heuristic, steps, depth, state, rows, cols, regionX, regionY, blocks, stopwatch, logger);
					if (isSolved == 1)
					{
						return isSolved;
					}

					(*depth)--;
					moveBackward(state, rows, cols, nextPos.first, nextPos.second);
				}
			}
		}

		return 0;
	}
}