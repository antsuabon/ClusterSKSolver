#pragma once
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <limits>
#include <sstream>

const int NORMAL = 0;
const int HEURISTIC1 = 1;
const int HEURISTIC2 = 2;
const int HEURISTIC3 = 3;

std::pair<int, int> findNextZero(int *state, int n);
std::pair<int, int> findNextZeroByBenefit(int *state, int regionX, int regionY, int n, std::map<std::vector<std::pair<int, int>>, int> blocks);
std::pair<int, int> findNextZeroBySum(int *state, int regionX, int regionY, int n, std::map<std::vector<std::pair<int, int>>, int> blocks);
std::pair<int, int> findNextZeroBy45Rule(int *state, int regionX, int regionY, int n, std::map<std::vector<std::pair<int, int>>, int> blocks);
std::pair<int, int> findNextPosition(int heuristic, int *state, int n, int regionX, int regionY, std::map<std::vector<std::pair<int, int>>, int> blocks);

std::vector<int> getAlternatives(int n);

bool isSolution(int *state, int n);
bool isSafe(int *state, int n, int regionX, int regionY, std::map<std::vector<std::pair<int, int>>, int> blocks, int newI, int newJ, int alternative);

void moveForward(int *state, int n, int newI, int newJ, int alternative);
void moveBackward(int *state, int n, int newI, int newJ);

int countZeros(int *state, int n);
void printState(int *state, int n, int regionX, int regionY);
std::string printStateLog(int *state, int n);