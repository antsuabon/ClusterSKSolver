#include "../header/SKLoader.h"

using namespace std;

vector<string> tokenize(string s, string del = " ")
{
	vector<string> res;

	int start = 0;
	int end = s.find(del);
	while (end != -1) {
		res.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}
	res.push_back(s.substr(start, end - start));

	return res;
}

void loadSudoku(string path, int** sudokuArray, int* rows, int* cols, int* regionX, int* regionY) 
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

				for each (string var1 in tokens1)
				{
					//cout << var1 << endl;

					if (var1.find("|") == string::npos)
					{
						
					}
					else
					{
						vector<string> tokens2 = tokenize(var1, "|");
						for each (string var2 in tokens2)
						{

							
						}
					}

				}

			}
			else
			{
				vector<int> row;
				vector<string> tokens = tokenize(line, ",");

				
				for each (string var in tokens)
				{
					row.push_back(stoi(var));
				}

				initialState.push_back(row);
			}
		}
	}

	(*rows) = initialState.size();
	(*cols) = initialState[0].size();

	(*sudokuArray) = new int[(*rows) * (*cols)];
	for (size_t i = 0; i < (*rows); i++)
	{
		for (size_t j = 0; j < (*cols); j++)
		{
			(*sudokuArray)[i * (*cols) + j] = initialState[i][j];
		}
	}
}