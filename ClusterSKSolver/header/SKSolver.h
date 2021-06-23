#pragma once
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "../header/SKUtils.h"

namespace SKSolver
{
    const int NORMAL = 0;
    const int BENEFIT = 1;
    const int MIN_SECTOR = 2;
    const int RULE_45 = 3;

    int solveSudoku(int heuristic, int *steps, int *depth, int *state, int n, int regionX, int regionY, std::map<std::vector<std::pair<int, int>>, int> blocks, spdlog::stopwatch sw, std::shared_ptr<spdlog::logger> logger);
}
