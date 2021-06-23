#pragma once
#include <iostream>
#include <stdlib.h>

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
    const int BENEFIT = 1;
    const int MIN_SECTOR = 2;
    const int RULE_45 = 3;

    int solveSudoku(int rank, int size, int heuristic, int initialMaxDepth, int *steps, int *state, int n, int regionX, int regionY, std::map<std::vector<std::pair<int, int>>, int> blocks, spdlog::stopwatch sw, std::shared_ptr<spdlog::logger> logger);
}
