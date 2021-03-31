#pragma once

#include <iostream>

#include <vector>
#include <string>
#include <fstream>

void loadSudoku(std::string path, int** sudokuArray, int* rows, int* cols, int* regionX, int* regionY);