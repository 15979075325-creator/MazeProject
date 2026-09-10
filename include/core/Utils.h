#pragma once

#include "Common.h"

#include <chrono>
#include <random>
#include <vector>

std::mt19937& rng();

void setSeed(unsigned int seed);

class ScopedTimer {
private:
    const char* name;
    std::chrono::steady_clock::time_point startTime;

public:
    explicit ScopedTimer(const char* timerName);

    ~ScopedTimer();
};

void printMaze(
    const Grid& grid,
    const std::vector<Pos>& path,
    Pos start,
    Pos goal
);
