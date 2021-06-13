#pragma once

#include <iostream>

#include <vector>
#include <string>
#include <map>
#include <utility>
#include <fstream>

void loadSudoku(std::string path, int **sudokuArray, std::map<std::vector<std::pair<int, int>>, int> *blocks, int *n, int *regionX, int *regionY);