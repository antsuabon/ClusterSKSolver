#pragma once
#include <iostream>

#include <vector>
#include <queue>
#include <mpi.h>

#include <spdlog/spdlog.h>

#include "../header/SKUtils.h"

const int NORMAL = 0;
const int HEURISTIC1 = 1;
const int HEURISTIC2 = 2;
const int HEURISTIC3 = 3;

int solveSudoku(int rank, int size, int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY);