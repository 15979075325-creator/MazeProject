#include <iostream>
#include <vector>

#include "core/Utils.h"
#include "generator/DfsGenerator.h"
#include "solver/Solver.h"

int main() {
    ScopedTimer timer("Generate and solve maze");

    DfsGenerator generator(21, 21);
    generator.generate();

    const Grid& grid = generator.grid();
    Pos start = generator.start();
    Pos goal = generator.goal();

    std::vector<Pos> path = bfsFind(grid, start, goal);

    if (path.empty()) {
        std::cout << "No path found.\n";
        return 1;
    }

    std::cout << "Path length: " << path.size() - 1 << "\n\n";
    printMaze(grid, path, start, goal);

    return 0;
}
