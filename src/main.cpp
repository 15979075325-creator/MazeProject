#include <iostream>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

#include "core/Utils.h"
#include "core/FileIO.h"
#include "generator/DfsGenerator.h"
#include "model/Maze.h"
#include "solver/Solver.h"

static void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

int main() {
    DfsGenerator generator(21, 21);
    generator.generate();

    Maze maze(generator.grid());
    Pos start = generator.start();
    Pos goal = generator.goal();
    std::vector<Pos> path;
    std::string message;

    while (true) {
        clearScreen();
        printMaze(maze.grid(), path, start, goal);
        if (!message.empty()) std::cout << '\n' << message << '\n';

        std::cout << "\n1. Edit a cell\n"
                  << "2. Find path with BFS\n"
                  << "3. Save maze\n"
                  << "4. Load maze\n"
                  << "0. Exit\n"
                  << "Choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) break;

            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            message = "Please enter a number.";
            continue;
        }

        if (choice == 0) break;

        if (choice == 1) {
            int row, col, type;

            std::cout << "Enter row col type (0=road, 1=wall): ";

            if (!(std::cin >> row >> col >> type)) {
                if (std::cin.eof()) break;

                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                );
                message = "Invalid input.";
                continue;
            }

            if (!maze.inBounds(row, col)) {
                message = "Coordinates out of bounds.";
                continue;
            }

            if (Pos{row, col} == start || Pos{row, col} == goal) {
                message = "Cannot edit start or goal.";
                continue;
            }

            if (type != 0 && type != 1) {
                message = "Type must be 0 or 1.";
                continue;
            }

            maze.setCell(row, col, type == 0 ? ' ' : '#');
            path.clear();
            message = "Cell updated. Find the path again.";

        } else if (choice == 2) {
            auto began = std::chrono::steady_clock::now();
            path = bfsFind(maze.grid(), start, goal);
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - began).count();

            if (path.empty()) {
                message = "No path found.";
            } else {
                message = "Path length: " + std::to_string(path.size() - 1);
            }
            message += " BFS: " + std::to_string(elapsed) + " us";

        } else if (choice == 3 || choice == 4) {
            std::cout << "Enter filename (for example maze.txt): ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string filename;
            if (!std::getline(std::cin, filename)) break;
            if (filename.empty()) {
                message = "Filename cannot be empty.";
                continue;
            }

            if (choice == 3) {
                Grid saved = maze.grid();
                saved[start.first][start.second] = 'S';
                saved[goal.first][goal.second] = 'E';
                message = saveMaze(saved, filename)
                    ? "Maze saved: " + filename
                    : "Save failed. Check the filename and folder permissions.";
                continue;
            }

            Grid loaded;
            if (!loadMaze(filename, loaded)) {
                message = "Load failed: file missing or invalid. Current maze kept.";
                continue;
            }

            Pos loadedStart{-1, -1}, loadedGoal{-1, -1};
            int startCount = 0, goalCount = 0;
            for (std::size_t row = 0; row < loaded.size(); ++row) {
                for (std::size_t col = 0; col < loaded[row].size(); ++col) {
                    char& cell = loaded[row][col];
                    if (cell == 'S') {
                        loadedStart = {static_cast<int>(row), static_cast<int>(col)};
                        ++startCount;
                    } else if (cell == 'E') {
                        loadedGoal = {static_cast<int>(row), static_cast<int>(col)};
                        ++goalCount;
                    }
                    // Stored route marks are stale after loading or editing.
                    if (cell == '.' || cell == 'S' || cell == 'E') cell = ' ';
                }
            }
            if (startCount != 1 || goalCount != 1) {
                message = "Load failed: need exactly one S and one E. Current maze kept.";
                continue;
            }

            maze = Maze(loaded);
            start = loadedStart;
            goal = loadedGoal;
            path.clear();
            message = "Maze loaded: " + filename + ". Find the path again.";

        } else {
            message = "Unknown choice.";
        }
    }

    return 0;
}
