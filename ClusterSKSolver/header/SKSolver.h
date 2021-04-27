#pragma once
#include <vector>

#include <spdlog/spdlog.h>

#include "../header/SKUtils.h"

namespace SKSolver 
{
    const int NORMAL = 0;
    const int HEURISTIC1 = 1;
    const int HEURISTIC2 = 2;
    const int HEURISTIC3 = 3;

    int solveSudoku(int heuristic, int* steps, int* state, int rows, int cols, int regionX, int regionY);
}
