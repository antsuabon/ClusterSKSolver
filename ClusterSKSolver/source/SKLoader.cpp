#include "../header/SKLoader.h"

using namespace std;

vector<string> tokenize(string s, string del = " ")
{
	vector<string> res;

	int start = 0;
	int end = s.find(del);
	while (end != -1)
	{
		res.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}
	res.push_back(s.substr(start, end - start));

	return res;
}

void loadSudoku(string path, int **sudokuArray, map<vector<pair<int, int>>, int> *blocks, int *n, int *regionX, int *regionY)
{

	ifstream infile(path);
	string line;
	string state = "";
	vector<vector<int>> initialState;

	while (std::getline(infile, line))
	{
		if (!line.empty())
		{
			if (line.find("r") != string::npos)
			{
				state = "r";
			}
			else if (line.find("b") != string::npos)
			{
				state = "b";
			}
			else if (state == "r")
			{
				vector<string> tokens = tokenize(line, "x");

				(*regionX) = stoi(tokens[0]);
				(*regionY) = stoi(tokens[1]);
			}
			else if (state == "b")
			{
				vector<string> tokens1 = tokenize(line, "->");

				vector<pair<int, int>> positions;

				vector<string> tokens2 = tokenize(tokens1[0], "|");
				for (string &var2 : tokens2)
				{
					vector<string> tokens3 = tokenize(var2, ",");
					positions.push_back({stoi(tokens3[0]), stoi(tokens3[1])});
				}

				blocks->insert({positions, stoi(tokens1[1])});
			}
			else
			{
				vector<int> row;
				vector<string> tokens = tokenize(line, ",");

				for (string &var : tokens)
				{
					row.push_back(stoi(var));
				}

				initialState.push_back(row);
			}
		}
	}

	(*n) = initialState.size();

	(*sudokuArray) = new int[(*n) * (*n)];
	for (size_t i = 0; i < (*n); i++)
	{
		for (size_t j = 0; j < (*n); j++)
		{
			(*sudokuArray)[i * (*n) + j] = initialState[i][j];
		}
	}
}