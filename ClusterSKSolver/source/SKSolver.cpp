#include "../header/SKSolver.h"

using namespace std;

namespace SKSolver
{
	int solveSudoku(int heuristic, int *steps, int *depth, int *state, int n, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, spdlog::stopwatch stopwatch, shared_ptr<spdlog::logger> logger)
	{
		(*steps)++;

		bool isSolved = isSolution(state, n);

		if (logger != nullptr)
		{
			logger->info("{}\t{}\t{:.4}\t{}\t{}", *steps, *depth, stopwatch, isSolved, printStateLog(state, n));
		}

		if (isSolved)
		{
			return 1;
		}
		else
		{
			pair<int, int> nextPos = findNextPosition(heuristic, state, n, regionX, regionY, blocks);

			for (int &alternative : getAlternatives(n))
			{
				if (isSafe(state, n, regionX, regionY, blocks, nextPos.first, nextPos.second, alternative))
				{
					moveForward(state, n, nextPos.first, nextPos.second, alternative);
					(*depth)++;

					int isSolved = solveSudoku(heuristic, steps, depth, state, n, regionX, regionY, blocks, stopwatch, logger);
					if (isSolved == 1)
					{
						return isSolved;
					}

					(*depth)--;
					moveBackward(state, n, nextPos.first, nextPos.second);
				}
			}
		}

		return 0;
	}
}