#pragma once
#include <iostream>
#include <stdlib.h>

#include <vector>
#include <stack>
#include <mpi.h>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "../header/SKUtils.h"

namespace DistributedSKSolver
{
    const int NORMAL = 0;
    const int BENEFIT = 1;
    const int MIN_SECTOR = 2;
    const int RULE_45 = 3;

    const int SOLVE_TAG = 0;
    const int BnB_TAG = 1;
    const int PBM_TAG = 2;
    const int IDLE_TAG = 3;
    const int DONE_TAG = 4;
    const int END_TAG = 5;

    int solveSudoku(int rank, int size, int heuristic, int refreshStep, int *steps, int *state, int n, int regionX, int regionY, std::map<std::vector<std::pair<int, int>>, int> blocks, spdlog::stopwatch sw, std::shared_ptr<spdlog::logger> logger);
}
