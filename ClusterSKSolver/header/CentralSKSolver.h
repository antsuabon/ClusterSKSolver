#pragma once
#include <iostream>

#include <vector>
#include <queue>
#include <mpi.h>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "../header/SKUtils.h"

namespace CentralSKSolver
{
    const int NORMAL = 0;
    const int HEURISTIC1 = 1;
    const int HEURISTIC2 = 2;
    const int HEURISTIC3 = 3;

    int solveSudoku(int rank, int size, int heuristic, double initialMaxDepth, int *steps, int *state, int rows, int cols, int regionX, int regionY, std::map<std::vector<std::pair<int, int>>, int> blocks, spdlog::stopwatch sw, std::shared_ptr<spdlog::logger> logger);
}
