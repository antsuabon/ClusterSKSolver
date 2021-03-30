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

bool checkRow(int* state, int rows, int cols, int y, int value)
{
	bool res = true;

	for (int j = 0; j < cols && res; j++)
	{
		res = state[y * cols + j] != value;
	}

	return res;
}

int countRowCompletedCells(int* state, int rows, int cols, int y)
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

bool checkColumn(int* state, int rows, int cols, int x, int value)
{
	bool res = true;

	for (int i = 0; i < rows && res; i++)
	{
		res = state[i * cols + x] != value;
	}

	return res;
}

int countColumnCompletedCells(int* state, int rows, int cols, int x)
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

bool checkRegion(int* state, int rows, int cols, int regionX, int regionY, int newI, int newJ, int value)
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

int countRegionCompletedCells(int* state, int rows, int cols, int regionX, int regionY, int newI, int newJ)
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

bool isSafe(int* state, int rows, int cols, int regionX, int regionY, int newI, int newJ, int alternative)
{
	return checkRow(state, rows, cols, newI, alternative)
		&& checkColumn(state, rows, cols, newJ, alternative)
		&& checkRegion(state, rows, cols, regionX, regionY, newI, newJ, alternative);
}

void moveForward(int* state, int rows, int cols, int newI, int newJ, int alternative)
{
	state[newI * cols + newJ] = alternative;
}

void moveBackward(int* state, int rows, int cols, int newI, int newJ)
{
	state[newI * cols + newJ] = 0;
}

bool isSolution(int* state, int rows, int cols)
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

pair<int, int> findNextZero(int* state, int rows, int cols)
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
				res = { i, j };
			}
		}

	}

	return res;
}

pair<int, int> findNextZeroByBenefit(int* state, int regionX, int regionY, int rows, int cols)
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
					countRegionCompletedCells(state, rows, cols, regionX, regionY, i, j);
				if (newBenefit > maxBenefit)
				{
					max = { i, j };
					maxBenefit = newBenefit;
				}
			}
		}

	}

	return max;
}


void printState(int* state, int rows, int cols)
{
	for (size_t i = 0; i < rows; i++)
	{
		std::string row = "\t";
		for (size_t j = 0; j < cols; j++)
		{
			row += to_string(state[i * cols + j]) + " ";
		}

		spdlog::info(row);
	}
}