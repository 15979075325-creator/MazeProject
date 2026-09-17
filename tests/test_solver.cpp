#include "solver/Solver.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

// 检查首尾、边界、是否穿墙，以及每一步是否相邻
static bool pathValid(
    const Grid& grid,
    const std::vector<Pos>& path,
    Pos start,
    Pos goal
) {
    if (path.empty()) return false;
    if (path.front() != start || path.back() != goal) return false;

    for (std::size_t i = 0; i < path.size(); ++i) {
        int row = path[i].first;
        int col = path[i].second;

        if (row < 0 || row >= static_cast<int>(grid.size())) {
            return false;
        }

        if (col < 0 || col >= static_cast<int>(grid[row].size())) {
            return false;
        }

        if (grid[row][col] == '#') return false;

        if (i > 0) {
            int distance =
                std::abs(row - path[i - 1].first) +
                std::abs(col - path[i - 1].second);

            if (distance != 1) return false;
        }
    }

    return true;
}

// 同时检查两种算法的最短路径
static void checkSolution(
    const Grid& grid,
    Pos start,
    Pos goal,
    std::size_t expectedCells
) {
    auto bfsPath = bfsFind(grid, start, goal);
    auto dijkstraPath = dijkstraFind(grid, start, goal);

    assert(bfsPath.size() == expectedCells);
    assert(dijkstraPath.size() == expectedCells);

    assert(pathValid(grid, bfsPath, start, goal));
    assert(pathValid(grid, dijkstraPath, start, goal));
}

// 直线：5 个格子，4 步
static void testStraightLine() {
    Grid grid(5, std::vector<char>(5, '#'));

    for (int col = 0; col < 5; ++col) {
        grid[2][col] = ' ';
    }

    grid[2][0] = 'S';
    grid[2][4] = 'E';

    checkSolution(grid, {2, 0}, {2, 4}, 5);
    std::cout << "[PASS] Straight line\n";
}

// 折线：7 个格子，6 步
static void testTurn() {
    Grid grid(3, std::vector<char>(5, '#'));

    for (int col = 0; col < 5; ++col) {
        grid[0][col] = ' ';
    }

    for (int row = 0; row < 3; ++row) {
        grid[row][4] = ' ';
    }

    checkSolution(grid, {0, 0}, {2, 4}, 7);
    std::cout << "[PASS] Turn\n";
}

// 无解
static void testNoPath() {
    Grid grid(3, std::vector<char>(5, ' '));

    for (int row = 0; row < 3; ++row) {
        grid[row][2] = '#';
    }

    assert(bfsFind(grid, {1, 0}, {1, 4}).empty());
    assert(dijkstraFind(grid, {1, 0}, {1, 4}).empty());

    std::cout << "[PASS] No path\n";
}

// 起点和终点相同：路径只包含一个格子
static void testSamePoint() {
    Grid grid(3, std::vector<char>(3, ' '));

    checkSolution(grid, {1, 1}, {1, 1}, 1);
    std::cout << "[PASS] Same point\n";
}

// 空迷宫、越界坐标、起点或终点为墙
static void testInvalidInput() {
    Grid empty;
    Grid grid(3, std::vector<char>(3, ' '));
    grid[0][0] = '#';

    for (auto findPath : {bfsFind, dijkstraFind}) {
        assert(findPath(empty, {0, 0}, {0, 0}).empty());
        assert(findPath(grid, {-1, 0}, {1, 1}).empty());
        assert(findPath(grid, {1, 1}, {3, 0}).empty());
        assert(findPath(grid, {0, 0}, {1, 1}).empty());
        assert(findPath(grid, {1, 1}, {0, 0}).empty());
        assert(findPath(Grid{{}}, {0, 0}, {0, 0}).empty());
        assert(findPath(Grid{{' ', ' '}, {' '}}, {0, 0}, {0, 1}).empty());
        assert(findPath(Grid{{' '}, {' ', ' '}}, {0, 0}, {1, 0}).empty());
    }

    std::cout << "[PASS] Invalid input\n";
}

int main() {
    testStraightLine();
    testTurn();
    testNoPath();
    testSamePoint();
    testInvalidInput();

    std::cout << "ALL TESTS PASSED\n";
    return 0;
}
