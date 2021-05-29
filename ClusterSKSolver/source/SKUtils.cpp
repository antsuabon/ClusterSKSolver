#include "../header/SKUtils.h"

using namespace std;

vector<int> getAlternatives(int rows, int cols)
{
	vector<int> res;

	for (int i = 1; i <= std::max(cols, rows); i++)
	{
		res.push_back(i);
	}

	return res;
}

bool checkRow(int *state, int rows, int cols, int y, int value)
{
	bool res = true;

	for (int j = 0; j < cols && res; j++)
	{
		res = state[y * cols + j] != value;
	}

	return res;
}

int countRowCompletedCells(int *state, int rows, int cols, int y)
{
	int res = 1;

	for (int j = 0; j < cols && res; j++)
	{
		if (state[y * cols + j] != 0)
		{
			res++;
		}
	}

	return res;
}

int sumRowCompletedCells(int *state, int rows, int cols, int y)
{
	int res = 0;

	for (int j = 0; j < cols && res; j++)
	{
		res += state[y * cols + j];
	}

	return res;
}

bool checkColumn(int *state, int rows, int cols, int x, int value)
{
	bool res = true;

	for (int i = 0; i < rows && res; i++)
	{
		res = state[i * cols + x] != value;
	}

	return res;
}

int countColumnCompletedCells(int *state, int rows, int cols, int x)
{
	int res = 1;

	for (int i = 0; i < rows; i++)
	{
		if (state[i * cols + x] != 0)
		{
			res++;
		}
	}

	return res;
}

int sumColumnCompletedCells(int *state, int rows, int cols, int x)
{
	int res = 0;

	for (int i = 0; i < rows; i++)
	{
		res += state[i * cols + x];
	}

	return res;
}

bool checkRegion(int *state, int rows, int cols, int regionX, int regionY, int newI, int newJ, int value)
{
	bool res = true;

	int initialX = (newJ / regionX) * regionX;
	for (int i = 0; i < regionY && res; i++)
	{
		int initialY = (newI / regionY) * regionY;
		for (int j = 0; j < regionX && res; j++)
		{
			res = state[(initialY + i) * cols + initialX + j] != value;
		}
	}

	return res;
}

int countRegionCompletedCells(int *state, int rows, int cols, int regionX, int regionY, int newI, int newJ)
{
	int res = 1;

	int initialX = (newJ / regionX) * regionX;
	for (int i = 0; i < regionY; i++)
	{
		int initialY = (newI / regionY) * regionY;
		for (int j = 0; j < regionX; j++)
		{
			if (state[(initialY + i) * cols + initialX + j] != 0)
			{
				res++;
			}
		}
	}

	return res;
}

int sumRegionCompletedCells(int *state, int rows, int cols, int regionX, int regionY, int newI, int newJ)
{
	int res = 0;

	int initialX = (newJ / regionX) * regionX;
	for (int i = 0; i < regionY; i++)
	{
		int initialY = (newI / regionY) * regionY;
		for (int j = 0; j < regionX; j++)
		{
			res += state[(initialY + i) * cols + initialX + j];
		}
	}

	return res;
}

bool checkBlock(int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, int newI, int newJ, int value)
{
	bool res = true;

	for (auto &item : blocks)
	{
		if (find(item.first.begin(), item.first.end(), pair<int, int>(newI, newJ)) != item.first.end())
		{
			int sum = 0;
			for (auto &pos : item.first)
			{
				sum += (newI == pos.first && newJ == pos.second) ? value : state[pos.first * cols + pos.second];
			}

			res = res && sum <= item.second;
		}
	}

	return res;
}

int countBlockCompletedCells(int *state, int rows, int cols, int regionX, int regionY, int newI, int newJ, map<vector<pair<int, int>>, int> blocks)
{
	int res = 1;

	for (auto &item : blocks)
	{
		if (find(item.first.begin(), item.first.end(), pair<int, int>(newI, newJ)) != item.first.end())
		{
			int sum = 0;
			for (auto &pos : item.first)
			{
				if (state[pos.first * cols + pos.second] != 0)
				{
					res++;
				}
			}
		}
	}

	return res;
}

bool isSafe(int *state, int rows, int cols, int regionX, int regionY, map<vector<pair<int, int>>, int> blocks, int newI, int newJ, int alternative)
{
	return checkRow(state, rows, cols, newI, alternative) && checkColumn(state, rows, cols, newJ, alternative) && checkRegion(state, rows, cols, regionX, regionY, newI, newJ, alternative) && checkBlock(state, rows, cols, regionX, regionY, blocks, newI, newJ, alternative);
}

void moveForward(int *state, int rows, int cols, int newI, int newJ, int alternative)
{
	state[newI * cols + newJ] = alternative;
}

void moveBackward(int *state, int rows, int cols, int newI, int newJ)
{
	state[newI * cols + newJ] = 0;
}

bool isSolution(int *state, int rows, int cols)
{
	bool found = false;

	for (int i = 0; i < rows && !found; i++)
	{
		for (int j = 0; j < cols && !found; j++)
		{
			found = state[i * cols + j] == 0;
		}
	}

	return !found;
}

pair<int, int> findNextZero(int *state, int rows, int cols)
{
	bool found = false;
	pair<int, int> res;

	for (int i = 0; i < rows && !found; i++)
	{
		for (int j = 0; j < cols && !found; j++)
		{
			found = state[i * cols + j] == 0;
			if (found)
			{
				res = {i, j};
			}
		}
	}

	return res;
}

pair<int, int> findNextZeroByBenefit(int *state, int regionX, int regionY, int rows, int cols, map<vector<pair<int, int>>, int> blocks)
{
	pair<int, int> max;
	double maxBenefit = 0.;
	double newBenefit = 0.;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (state[i * cols + j] == 0)
			{
				newBenefit = countRowCompletedCells(state, rows, cols, i) +
							 countColumnCompletedCells(state, rows, cols, j) +
							 countRegionCompletedCells(state, rows, cols, regionX, regionY, i, j) +
							 countBlockCompletedCells(state, rows, cols, regionX, regionY, i, j, blocks);
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

pair<int, int> findNextZeroBySum(int *state, int regionX, int regionY, int rows, int cols, map<vector<pair<int, int>>, int> blocks)
{
	pair<int, int> max;
	double maxBenefit = -1.;
	double maxSize = numeric_limits<double>::max();
	double newBenefit = -1.;
	double newSize = -1.;

	int res = 1;

	for (auto &item : blocks)
	{
		int sum = 0;
		for (auto &pos : item.first)
		{
			if (state[pos.first * cols + pos.second] == 0)
			{
				newBenefit = abs(8.5 - item.second);
				newSize = item.first.size();

				if (newBenefit > maxBenefit && newSize < maxSize)
				{
					max = {pos.first, pos.second};
					maxBenefit = newBenefit;
					maxSize = newSize;
				}
			}
		}
	}

	return max;
}

pair<int, int> findNextZeroBy45Rule(int *state, int regionX, int regionY, int rows, int cols, map<vector<pair<int, int>>, int> blocks)
{
	pair<int, int> best;
	double maxBenefit = -1.;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (state[i * cols + j] == 0)
			{
				int sumRow = sumRowCompletedCells(state, rows, cols, i);
				int sumColumn = sumColumnCompletedCells(state, rows, cols, j);
				int sumRegion = sumRegionCompletedCells(state, rows, cols, regionX, regionY, i, j);

				if (sumRow > maxBenefit || sumColumn > maxBenefit || sumRegion > maxBenefit)
				{
					best = {i, j};
					maxBenefit = max({sumRow, sumColumn, sumRegion});
				}
			}
		}
	}

	return best;
}

int countZeros(int *state, int rows, int cols)
{
	int res = 0;

	for (int i = 0; i < rows * cols; i++)
	{
		if (state[i] == 0)
		{
			res++;
		}
	}

	return res;
}

void printState(int *state, int rows, int cols, int regionX, int regionY)
{
	for (size_t i = 0; i < rows; i++)
	{

		ostringstream oss;
		oss << internal << setfill('-') << setw(regionX * 2 + regionX) << "";

		string separator = "";
		for (size_t i = 0; i < cols / regionX; i++)
		{
			if (i != 0 && i != cols - 1)
			{
				separator += oss.str() + "- ";
			}
			else
			{
				separator += oss.str() + " ";
			}
		}

		string row = "\t";
		for (size_t j = 0; j < cols; j++)
		{
			ostringstream oss;
			oss << internal << setfill(' ') << setw(2) << to_string(state[i * cols + j]);

			if (j % regionX == regionX - 1 && j != cols - 1)
			{
				row += oss.str() + " | ";
			}
			else
			{
				row += oss.str() + " ";
			}
		}

		spdlog::info(row);

		if (i % regionY == regionY - 1 && i != rows - 1)
		{

			spdlog::info("\t{}", separator);
		}
	}
}

string printStateLog(int *state, int rows, int cols)
{
	string res = "";
	int n = rows * cols;
	for (size_t i = 0; i < n; i++)
	{
		res += to_string(state[i]);
		if (i != n - 1)
		{
			res += ",";
		}
	}

	return res;
}