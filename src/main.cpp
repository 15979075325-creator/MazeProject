#include <iostream>
#include <vector>

#include "core/Utils.h"
#include "generator/DfsGenerator.h"
#include "solver/Solver.h"
#include "model/Maze.h"

int main() {
    DfsGenerator generator(21, 21);
    {
        ScopedTimer timer("Generation");
        generator.generate();
    }

    Maze maze(generator.grid());
    const Grid& grid = maze.grid();
    Pos start = generator.start();
    Pos goal = generator.goal();

    std::vector<Pos> path;
    {
        ScopedTimer timer("BFS");
        path = bfsFind(grid, start, goal);
    }

    if (path.empty()) {
        std::cout << "No path found.\n";
        return 1;
    }

    std::cout << "Path length: " << path.size() - 1 << "\n\n";
    printMaze(grid, path, start, goal);

    return 0;
}
