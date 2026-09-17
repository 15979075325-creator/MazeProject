#include "core/FileIO.h"
#include "core/Utils.h"
#include "generator/DfsGenerator.h"
#include "model/Maze.h"
#include "solver/Solver.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>

static void testGeneration() {
    for (unsigned seed = 0; seed < 10; ++seed) {
        for (Pos size : {Pos{3, 3}, Pos{15, 9}, Pos{21, 21}, Pos{41, 41}}) {
            setSeed(seed);
            DfsGenerator gen(size.first, size.second);
            gen.generate();
            Grid seen = gen.grid();
            int vertices = 0, edges = 0;
            for (int r = 0; r < size.second; ++r) {
                for (int c = 0; c < size.first; ++c) {
                    if (r == 0 || c == 0 || r == size.second - 1 || c == size.first - 1) {
                        assert(seen[r][c] == '#');
                    }
                    if (seen[r][c] == '#') continue;
                    ++vertices;
                    if (r + 1 < size.second && seen[r + 1][c] != '#') ++edges;
                    if (c + 1 < size.first && seen[r][c + 1] != '#') ++edges;
                }
            }
            std::queue<Pos> queue;
            queue.push(gen.start());
            seen[1][1] = '#';
            int reached = 0;
            while (!queue.empty()) {
                Pos p = queue.front(); queue.pop(); ++reached;
                for (Pos d : {Pos{-1, 0}, Pos{1, 0}, Pos{0, -1}, Pos{0, 1}}) {
                    int r = p.first + d.first, c = p.second + d.second;
                    if (r >= 0 && r < size.second && c >= 0 && c < size.first && seen[r][c] != '#') {
                        seen[r][c] = '#'; queue.push({r, c});
                    }
                }
            }
            assert(reached == vertices && edges == vertices - 1);
            auto a = bfsFind(gen.grid(), gen.start(), gen.goal());
            auto b = dijkstraFind(gen.grid(), gen.start(), gen.goal());
            assert(!a.empty() && a.size() == b.size());
        }
    }
    DfsGenerator reused(21, 21);
    setSeed(10); reused.generate();
    Grid first = reused.grid();
    setSeed(20); reused.generate();
    DfsGenerator fresh(21, 21);
    setSeed(20); fresh.generate();
    assert(reused.grid() == fresh.grid());
    assert(reused.grid() != first);
    setSeed(10); reused.generate();
    assert(reused.grid() == first);
    for (Pos size : {Pos{0, 3}, Pos{2, 3}, Pos{3, 4}}) {
        bool rejected = false;
        try { DfsGenerator invalid(size.first, size.second); }
        catch (const std::invalid_argument&) { rejected = true; }
        assert(rejected);
    }
    std::cout << "[PASS] Generation, regeneration and fixed seeds\n";
}

static void testModel() {
    for (Pos size : {Pos{-1, 3}, Pos{3, -1}, Pos{0, 3}}) {
        bool rejected = false;
        try { Maze invalid(size.first, size.second); }
        catch (const std::invalid_argument&) { rejected = true; }
        assert(rejected);
    }
    bool rejected = false;
    try { Maze invalid(Grid{{' ', ' '}, {' '}}); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
    Grid input{{'#', '#', '#'}, {'S', ' ', 'E'}};
    Maze maze(input);
    assert(maze.height() == 2 && maze.width() == 3 && maze.grid() == input);
    maze.setCell(1, 1, '.');
    maze.setCell(-1, 0, ' ');
    assert(maze.cell(1, 1) == '.' && maze.cell(-1, 0) == '#' && maze.isWall(2, 0));
    assert(input[1][1] == ' ');
    std::cout << "[PASS] Maze dimensions, imported grid and bounds\n";
}

static void testFileIO() {
    const std::string filename = "maze_fileio_test_" + std::to_string(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) + ".tmp";
    Grid expected{{'S', ' ', ' '}, {'#', '.', 'E'}};
    Grid output;
    assert(!loadMaze(filename, output));
    assert(saveMaze(expected, filename));
    assert(loadMaze(filename, output) && output == expected);
    assert(!saveMaze(Grid{{' '}, {' ', ' '}}, filename));
    assert(!saveMaze(Grid{{'X'}}, filename));
    for (const char* invalid : {"2 3\nS  \n", "1 3\nS \n", "1 1\nX\n", "0 1\n", "1 1 extra\nS\n", "1 1\nS\nE\n"}) {
        { std::ofstream file(filename, std::ios::binary); file << invalid; }
        output = expected;
        assert(!loadMaze(filename, output) && output == expected);
    }
    { std::ofstream file(filename, std::ios::binary); file << "2 3\r\nS  \r\n#.E\r\n"; }
    assert(loadMaze(filename, output) && output == expected);
    assert(std::remove(filename.c_str()) == 0);
    std::cout << "[PASS] File IO, spaces, CRLF and invalid files\n";
}

int main() {
    testGeneration();
    testModel();
    testFileIO();
    std::cout << "ALL PROJECT TESTS PASSED\n";
}
