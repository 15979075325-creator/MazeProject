#include "../../include/core/Utils.h"

#include <iostream>

std::mt19937& rng() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

void setSeed(unsigned int seed) {
    rng().seed(seed);
}

ScopedTimer::ScopedTimer(const char* timerName)
    : name(timerName),
      startTime(std::chrono::steady_clock::now()) {
}

ScopedTimer::~ScopedTimer() {
    auto endTime = std::chrono::steady_clock::now();

    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime
        ).count();

    std::cout << name
              << ": "
              << elapsed
              << " ms\n";
}
void printMaze(
    const Grid& grid,
    const std::vector<Pos>& path,
    Pos start,
    Pos goal
) {
    if (grid.empty()) {
        return;
    }

    Grid display = grid;

    for (const Pos& position : path) {
        int row = position.first;
        int col = position.second;

        if (row >= 0 &&
            row < static_cast<int>(display.size()) &&
            col >= 0 &&
            col < static_cast<int>(display[row].size())) {
            display[row][col] = '.';
        }
    }

    if (start.first >= 0 &&
        start.first < static_cast<int>(display.size()) &&
        start.second >= 0 &&
        start.second < static_cast<int>(display[start.first].size())) {
        display[start.first][start.second] = 'S';
    }

    if (goal.first >= 0 &&
        goal.first < static_cast<int>(display.size()) &&
        goal.second >= 0 &&
        goal.second < static_cast<int>(display[goal.first].size())) {
        display[goal.first][goal.second] = 'E';
    }

    for (const auto& row : display) {
        for (char cell : row) {
            std::cout << cell;
        }

        std::cout << '\n';
    }
}
