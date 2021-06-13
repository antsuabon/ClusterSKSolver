#include "../header/SKUtils.h"

using namespace std;

vector<int> getAlternatives(int n)
{
	vector<int> res;

	for (int i = 1; i <= n; i++)
	{
		res.push_back(i);
	}

	return res;
}

bool checkRow(int *state, int n, int y, int value)
{
	bool res = true;

	for (int j = 0; j < n && res; j++)
	{
		res = state[y * n + j] != value;
	}

	return res;
}

float measureRowBenefit(int *state, int n, int y)
{
	float res = 1.0;

	for (int j = 0; j < n && res; j++)
	{
		if (state[y * n + j] != 0)
		{
			res++;
		}
	}

	return res / n;
}

int sumRowCompletedCells(int *state, int n, int y)
{
	int res = 0;

	for (int j = 0; j < n && res; j++)
	{
		res += state[y * n + j];
	}

	return res;
}

bool checkColumn(int *state, int n, int x, int value)
{
	bool res = true;

	for (int i = 0; i < n && res; i++)
	{
		res = state[i * n + x] != value;
	}

	return res;
}

float measureColumnBenefit(int *state, int n, int x)
{
	float res = 1;

	for (int i = 0; i < n; i++)
	{
		if (state[i * n + x] != 0)
		{
			res++;
		}
	}

	return res / n;
}

int sumColumnCompletedCells(int *state, int n, int x)
{
	int res = 0;

	for (int i = 0; i < n; i++)
	{
		res += state[i * n + x];
	}

	return res;
}

bool checkRegion(int *state, int n, int regionX, int regionY, int newI, int newJ, int value)
{
	bool res = true;

	int initialX = (newJ / regionX) * regionX;
	for (int i = 0; i < regionY && res; i++)
	{
		int initialY = (newI / regionY) * regionY;
		for (int j = 0; j < regionX && res; j++)
		{
			res = state[(initialY + i) * n + initialX + j] != value;
		}
	}

	return res;
}

float measureRegionBenefit(int *state, int n, int regionX, int regionY, int newI, int newJ)
{
	float res = 1.0;

	int initialX = (newJ / regionX) * regionX;
	for (int i = 0; i < regionY; i++)
	{
		int initialY = (newI / regionY) * regionY;
		for (int j = 0; j < regionX; j++)
		{
			if (state[(initialY + i) * n + initialX + j] != 0)
			{
				res++;
			}
		}
	}

	return res / n;
}

int sumRegionCompletedCells(int *state, int n, int regionX, int regionY, int newI, int newJ)
{
	int res = 0;

	int initialX = (newJ / regionX) * regionX;
	for (int i = 0; i < regionY; i++)
	{
		int initialY = (newI / regionY) * regionY;
		for (int j = 0; j < regionX; j++)
		{
			res += state[(initialY + i) * n + initialX + j];
		}
	}

	return res;
}

bool checkBlock(int *state, int n, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, int newI, int newJ, int value)
{
	bool res = true;

	for (auto &item : blocks)
	{
		if (find(item.first.begin(), item.first.end(), pair<int, int>(newI, newJ)) != item.first.end())
		{
			int sum = 0;
			bool anyEmpty = false;
			for (auto &pos : item.first)
			{
				if (!anyEmpty)
				{
					anyEmpty = state[pos.first * n + pos.second] == 0;
				}

				sum += (newI == pos.first && newJ == pos.second) ? value : state[pos.first * n + pos.second];
			}

			// res = res && sum <= item.second;
			res = anyEmpty ? res && sum <= item.second : res && sum == item.second;
		}
	}

	return res;
}

float measureBlockBenefit(int *state, int n, int regionX, int regionY, int newI, int newJ, map<vector<pair<int, int>>, int> blocks)
{
	float res = 1.0;

	for (auto &item : blocks)
	{
		if (find(item.first.begin(), item.first.end(), pair<int, int>(newI, newJ)) != item.first.end())
		{
			int sum = 0;
			for (auto &pos : item.first)
			{
				if (state[pos.first * n + pos.second] != 0)
				{
					res++;
				}
			}

			res /= item.first.size();
			break;
		}
	}

	return res;
}

bool isSafe(int *state, int n, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, int newI, int newJ, int alternative)
{
	return checkRow(state, n, newI, alternative) && checkColumn(state, n, newJ, alternative) && checkRegion(state, n, regionX, regionY, newI, newJ, alternative) && checkBlock(state, n, regionX, regionY, blocks, newI, newJ, alternative);
}

void moveForward(int *state, int n, int newI, int newJ, int alternative)
{
	state[newI * n + newJ] = alternative;
}

void moveBackward(int *state, int n, int newI, int newJ)
{
	state[newI * n + newJ] = 0;
}

bool isSolution(int *state, int n)
{
	bool found = false;

	for (int i = 0; i < n && !found; i++)
	{
		for (int j = 0; j < n && !found; j++)
		{
			found = state[i * n + j] == 0;
		}
	}

	return !found;
}

pair<int, int> findNextZero(int *state, int n)
{
	bool found = false;
	pair<int, int> res;

	for (int i = 0; i < n && !found; i++)
	{
		for (int j = 0; j < n && !found; j++)
		{
			found = state[i * n + j] == 0;
			if (found)
			{
				res = {i, j};
			}
		}
	}

	return res;
}

pair<int, int> findNextZeroByBenefit(int *state, int regionX, int regionY, int n, map<vector<pair<int, int>>, int> blocks)
{
	pair<int, int> max;
	double maxBenefit = 0.;
	double newBenefit = 0.;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (state[i * n + j] == 0)
			{
				newBenefit = measureRowBenefit(state, n, i) +
							 measureColumnBenefit(state, n, j) +
							 measureRegionBenefit(state, n, regionX, regionY, i, j) +
							 measureBlockBenefit(state, n, regionX, regionY, i, j, blocks);
				if (newBenefit > maxBenefit)
				{
					max = {i, j};
					maxBenefit = newBenefit;
				}
			}
		}
	}

	return max;
}

pair<int, int> findNextZeroBySum(int *state, int regionX, int regionY, int n, map<vector<pair<int, int>>, int> blocks)
{
	pair<int, int> bestPosition;
	double maxBenefit = -1.;
	double maxSize = numeric_limits<double>::max();
	double newBenefit = -1.;
	double newSize = -1.;

	int middle = (2 * n + 5) / 4;

	int res = 1;

	for (auto &item : blocks)
	{
		int sum = 0;
		for (auto &pos : item.first)
		{
			if (state[pos.first * n + pos.second] == 0)
			{
				newBenefit = abs(middle - item.second);
				newSize = item.first.size();

				if (newBenefit > maxBenefit && newSize < maxSize)
				{
					bestPosition = {pos.first, pos.second};
					maxBenefit = newBenefit;
					maxSize = newSize;
				}
			}
		}
	}

	return bestPosition;
}

pair<int, int> findNextZeroBy45Rule(int *state, int regionX, int regionY, int n, map<vector<pair<int, int>>, int> blocks)
{
	pair<int, int> best;
	double maxSum = -1.;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (state[i * n + j] == 0)
			{
				int sumRow = sumRowCompletedCells(state, n, i);
				int sumColumn = sumColumnCompletedCells(state, n, j);
				int sumRegion = sumRegionCompletedCells(state, n, regionX, regionY, i, j);

				if (sumRow > maxSum || sumColumn > maxSum || sumRegion > maxSum)
				{
					best = {i, j};
					maxSum = max({sumRow, sumColumn, sumRegion});
				}
			}
		}
	}

	return best;
}

pair<int, int> findNextPosition(int heuristic, int *state, int n, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks)
{
	pair<int, int> nextPos;

	switch (heuristic)
	{
	case NORMAL:
		nextPos = findNextZero(state, n);
		break;
	case HEURISTIC1:
		nextPos = findNextZeroByBenefit(state, regionX, regionY, n, blocks);
		break;
	case HEURISTIC2:
		nextPos = findNextZeroBySum(state, regionX, regionY, n, blocks);
		break;
	case HEURISTIC3:
		nextPos = findNextZeroBy45Rule(state, regionX, regionY, n, blocks);
		break;
	default:
		break;
	}

	return nextPos;
}

int countZeros(int *state, int n)
{
	int res = 0;

	for (int i = 0; i < n * n; i++)
	{
		if (state[i] == 0)
		{
			res++;
		}
	}

	return res;
}

void printState(int *state, int n, int regionX, int regionY)
{
	for (size_t i = 0; i < n; i++)
	{

		ostringstream oss;
		oss << internal << setfill('-') << setw(regionX * 2 + regionX) << "";

		string separator = "";
		for (size_t i = 0; i < n / regionX; i++)
		{
			if (i != 0 && i != n - 1)
			{
				separator += oss.str() + "- ";
			}
			else
			{
				separator += oss.str() + " ";
			}
		}

		string row = "\t";
		for (size_t j = 0; j < n; j++)
		{
			ostringstream oss;
			oss << internal << setfill(' ') << setw(2) << to_string(state[i * n + j]);

			if (j % regionX == regionX - 1 && j != n - 1)
			{
				row += oss.str() + " | ";
			}
			else
			{
				row += oss.str() + " ";
			}
		}

		spdlog::info(row);

		if (i % regionY == regionY - 1 && i != n - 1)
		{

			spdlog::info("\t{}", separator);
		}
	}
}

string printStateLog(int *state, int n)
{
	string res = "";
	for (size_t i = 0; i < n * n; i++)
	{
		res += to_string(state[i]);
		if (i != (n * n) - 1)
		{
			res += ",";
		}
	}

	return res;
}