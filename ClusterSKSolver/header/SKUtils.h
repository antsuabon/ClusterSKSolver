#pragma once
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <limits>

std::pair<int, int> findNextZero(int *state, int rows, int cols);
std::pair<int, int> findNextZeroByBenefit(int *state, int regionX, int regionY, int rows, int cols, std::map<std::vector<std::pair<int, int>>, int> blocks);
std::pair<int, int> findNextZeroBySum(int *state, int regionX, int regionY, int rows, int cols, std::map<std::vector<std::pair<int, int>>, int> blocks);
std::pair<int, int> findNextZeroBy45Rule(int *state, int regionX, int regionY, int rows, int cols, std::map<std::vector<std::pair<int, int>>, int> blocks);

std::vector<int> getAlternatives(int rows, int cols);

bool isSolution(int *state, int rows, int cols);
bool isSafe(int *state, int rows, int cols, int regionX, int regionY, std::map<std::vector<std::pair<int, int>>, int> blocks, int newI, int newJ, int alternative);

void moveForward(int *state, int rows, int cols, int newI, int newJ, int alternative);
void moveBackward(int *state, int rows, int cols, int newI, int newJ);

int countZeros(int *state, int rows, int cols);
void printState(int *state, int rows, int cols, int regionX, int regionY);
std::string printStateLog(int *state, int rows, int cols);