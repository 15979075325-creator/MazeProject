#include <iostream>
#include <vector>

#include "../include/core/Utils.h"
#include "../include/model/Maze.h"

int main() {
    {
        ScopedTimer timer("Maze test");

        Maze maze(9, 9);

        maze.setCell(1, 1, 'S');

        for (int col = 2; col <= 7; col++) {
            maze.setCell(1, col, ' ');
        }

        maze.setCell(1, 7, 'E');

        std::cout << "Maze:\n";
        maze.print();

        std::cout << "\nPath test:\n";

        Grid grid(5, std::vector<char>(5, '#'));

        std::vector<Pos> path = {
            {1, 1},
            {1, 2},
            {1, 3}
        };

        printMaze(
            grid,
            path,
            {1, 1},
            {1, 3}
        );
    }

    return 0;
}
