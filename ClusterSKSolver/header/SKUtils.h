#pragma once
#include <vector>
#include <string>
#include <spdlog/spdlog.h>

std::pair<int, int> findNextZero(int* state, int rows, int cols);
std::pair<int, int> findNextZeroByBenefit(int* state, int regionX, int regionY, int rows, int cols);

std::vector<int> getAlternatives(int rows, int cols);

bool isSolution(int* state, int rows, int cols);
bool isSafe(int* state, int rows, int cols, int regionX, int regionY, int newI, int newJ, int alternative);

void moveForward(int* state, int rows, int cols, int newI, int newJ, int alternative);
void moveBackward(int* state, int rows, int cols, int newI, int newJ);

void printState(int* state, int rows, int cols);