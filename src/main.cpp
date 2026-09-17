#include <iostream>
#include <chrono>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

#include "core/Utils.h"
#include "core/FileIO.h"
#include "generator/DfsGenerator.h"
#include "model/Maze.h"
#include "solver/Solver.h"
#include "solver/AStar.h"

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
    Pos end = generator.goal();
    std::vector<Pos> path;
    std::string message;

    while (true) {
        clearScreen();
        printMaze(maze.grid(), path, start, end);
        if (!message.empty()) std::cout << '\n' << message << '\n';

        std::cout << "\n--- Maze setup ---\n"
                  << "1. Generate a new maze (custom size)\n"
                  << "2. Edit a cell\n"
                  << "3. Set start and end\n"
                  << "\n--- Pathfinding ---\n"
                  << "4. Find path with BFS\n"
                  << "5. Find path with Dijkstra\n"
                  << "6. Find path with A*\n"
                  << "7. Compare BFS, Dijkstra and A*\n"
                  << "\n--- Files ---\n"
                  << "8. Save maze\n"
                  << "9. Load maze\n"
                  << "\n0. Exit\n"
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

        if (choice == 2) {
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

            if (Pos{row, col} == start || Pos{row, col} == end) {
                message = "Cannot edit start or end.";
                continue;
            }

            if (type != 0 && type != 1) {
                message = "Type must be 0 or 1.";
                continue;
            }

            maze.setCell(row, col, type == 0 ? ' ' : '#');
            path.clear();
            message = "Cell updated. Find the path again.";

        } else if (choice == 4 || choice == 5 || choice == 6) {
            const std::string algorithm = choice == 4 ? "BFS"
                : choice == 6 ? "A*" : "Dijkstra";
            auto began = std::chrono::steady_clock::now();
            if (choice == 4) {
                path = bfsFind(maze.grid(), start, end);
            } else if (choice == 6) {
                path = aStarFind(maze.grid(), start, end);
            } else {
                path = dijkstraFind(maze.grid(), start, end);
            }
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - began).count();

            if (path.empty()) {
                message = "No path found.";
            } else {
                message = "Path length: " + std::to_string(path.size() - 1);
            }
            message += " " + algorithm + ": " + std::to_string(elapsed) + " us";

        } else if (choice == 8 || choice == 9) {
            std::cout << "Enter filename (for example maze.txt): ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string filename;
            if (!std::getline(std::cin, filename)) break;
            if (filename.empty()) {
                message = "Filename cannot be empty.";
                continue;
            }

            if (choice == 8) {
                Grid saved = maze.grid();
                saved[start.first][start.second] = 'S';
                saved[end.first][end.second] = 'E';
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

            Pos loadedStart{-1, -1}, loadedEnd{-1, -1};
            int startCount = 0, endCount = 0;
            for (std::size_t row = 0; row < loaded.size(); ++row) {
                for (std::size_t col = 0; col < loaded[row].size(); ++col) {
                    char& cell = loaded[row][col];
                    if (cell == 'S') {
                        loadedStart = {static_cast<int>(row), static_cast<int>(col)};
                        ++startCount;
                    } else if (cell == 'E') {
                        loadedEnd = {static_cast<int>(row), static_cast<int>(col)};
                        ++endCount;
                    }
                    // Stored route marks are stale after loading or editing.
                    if (cell == '.' || cell == 'S' || cell == 'E') cell = ' ';
                }
            }
            if (startCount != 1 || endCount != 1) {
                message = "Load failed: need exactly one S and one E. Current maze kept.";
                continue;
            }

            maze = Maze(loaded);
            start = loadedStart;
            end = loadedEnd;
            path.clear();
            message = "Maze loaded: " + filename + ". Find the path again.";

        } else if (choice == 1) {
            int rows, cols;
            std::cout << "Enter rows cols (odd numbers, 3 to 51): ";
            if (!(std::cin >> rows >> cols)) {
                if (std::cin.eof()) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                message = "Invalid size input. Current maze kept.";
                continue;
            }
            if (rows < 3 || rows > 51 || cols < 3 || cols > 51 ||
                rows % 2 == 0 || cols % 2 == 0) {
                message = "Rows and cols must be odd numbers from 3 to 51. Current maze kept.";
                continue;
            }

            // Generator takes width (columns) before height (rows).
            DfsGenerator newGenerator(static_cast<unsigned>(cols),
                                      static_cast<unsigned>(rows));
            newGenerator.generate();
            maze = Maze(newGenerator.grid());
            start = newGenerator.start();
            end = newGenerator.goal();
            path.clear();
            message = "New maze generated: " + std::to_string(rows) + " rows x " +
                std::to_string(cols) + " cols. Find the path again.";

        } else if (choice == 3) {
            Pos newStart, newEnd;
            std::cout << "Enter start row col and end row col (0-based): ";
            if (!(std::cin >> newStart.first >> newStart.second
                          >> newEnd.first >> newEnd.second)) {
                if (std::cin.eof()) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                message = "Invalid coordinates. Start and end kept.";
                continue;
            }
            if (!maze.inBounds(newStart.first, newStart.second) ||
                !maze.inBounds(newEnd.first, newEnd.second)) {
                message = "Coordinates out of bounds. Start and end kept.";
                continue;
            }
            if (newStart == newEnd) {
                message = "Start and end must be different. Start and end kept.";
                continue;
            }
            if (maze.isWall(newStart.first, newStart.second) ||
                maze.isWall(newEnd.first, newEnd.second)) {
                message = "Start and end must be on roads. Start and end kept.";
                continue;
            }
            start = newStart;
            end = newEnd;
            path.clear();
            message = "Start updated to (" + std::to_string(start.first) + ", " +
                std::to_string(start.second) + "), end updated to (" +
                std::to_string(end.first) + ", " + std::to_string(end.second) +
                "). Find the path again.";

        } else if (choice == 7) {
            using FindPath = std::vector<Pos> (*)(const Grid&, Pos, Pos);
            const FindPath algorithms[] = {bfsFind, dijkstraFind, aStarFind};
            const char* names[] = {"BFS", "Dijkstra", "A*"};
            std::vector<Pos> results[3];
            message = "Algorithm comparison (length in steps, time in us):\n";
            for (int i = 0; i < 3; ++i) {
                auto began = std::chrono::steady_clock::now();
                results[i] = algorithms[i](maze.grid(), start, end);
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - began).count();
                message += std::string(names[i]) + ": ";
                message += results[i].empty()
                    ? "No path"
                    : "Path length: " + std::to_string(results[i].size() - 1);
                message += ", time: " + std::to_string(elapsed) + " us\n";
            }
            if (results[0].size() != results[1].size() ||
                results[0].size() != results[2].size()) {
                path.clear();
                message += "WARNING: Algorithm results disagree. Route not displayed.";
            } else if (results[0].empty()) {
                path.clear();
                message += "All three algorithms agree: no path.";
            } else {
                path = results[0];
                message += "All three path lengths match. Displaying the BFS route.";
            }

        } else {
            message = "Unknown choice.";
        }
    }

    return 0;
}
