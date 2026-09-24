#include "ui/ConsoleUI.h"

#include "core/Utils.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <conio.h>
#else
#include <thread>
#endif

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

static int readEditorKey() {
#ifdef _WIN32
    int key = _getch();
    if (key == 0 || key == 224) {
        int arrow = _getch();
        if (arrow == 72) return 'w';
        if (arrow == 80) return 's';
        if (arrow == 75) return 'a';
        if (arrow == 77) return 'd';
        return 0;
    }
    return key;
#else
    return std::cin.get();
#endif
}

bool keyboardEdit(Maze& maze, std::vector<Pos>& path,
                  Pos start, Pos end) {
    Pos cursor = start;
    bool changed = false;
    std::string status = "Move the cursor, then press SPACE to toggle a cell.";

#ifndef _WIN32
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
#endif

    while (true) {
        clearScreen();
        Grid display = maze.grid();
        for (const Pos& p : path) {
            if (maze.inBounds(p.first, p.second) &&
                display[p.first][p.second] != '#') {
                display[p.first][p.second] = '.';
            }
        }
        display[start.first][start.second] = 'S';
        display[end.first][end.second] = 'E';
        display[cursor.first][cursor.second] = '@';
        for (const auto& row : display) {
            for (char cell : row) std::cout << cell;
            std::cout << '\n';
        }

        std::cout << "\nKeyboard editor: W/A/S/D or arrow keys = move, "
                  << "SPACE = wall/road, Q = return\n"
                  << "Cursor: (" << cursor.first << ", " << cursor.second
                  << ")  @ = cursor\n"
                  << status << '\n';

        int key = readEditorKey();
        if (key == 'q' || key == 'Q') return changed;

        int nextRow = cursor.first;
        int nextCol = cursor.second;
        if (key == 'w' || key == 'W') --nextRow;
        else if (key == 's' || key == 'S') ++nextRow;
        else if (key == 'a' || key == 'A') --nextCol;
        else if (key == 'd' || key == 'D') ++nextCol;
        else if (key == ' ') {
            if (cursor == start || cursor == end) {
                status = "Start and end cannot be changed.";
            } else {
                maze.setCell(cursor.first, cursor.second,
                             maze.isWall(cursor.first, cursor.second) ? ' ' : '#');
                path.clear();
                changed = true;
                status = "Cell toggled. The old path was cleared.";
            }
            continue;
        } else {
            status = "Use W/A/S/D, arrow keys, SPACE or Q.";
            continue;
        }

        if (maze.inBounds(nextRow, nextCol)) {
            cursor = {nextRow, nextCol};
            status = "Cursor moved.";
        } else {
            status = "The cursor cannot move outside the maze.";
        }
    }
}

void animatePath(const Grid& grid, const std::vector<Pos>& path,
                 Pos start, Pos end) {
    std::vector<Pos> visiblePath;
    visiblePath.reserve(path.size());

    for (std::size_t i = 0; i < path.size(); ++i) {
        visiblePath.push_back(path[i]);
        clearScreen();
        printMaze(grid, visiblePath, start, end);
        std::cout << "\nAnimating path: " << (i + 1) << " / " << path.size()
                  << std::flush;
        if (i + 1 < path.size()) {
#ifdef _WIN32
            Sleep(40);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
#endif
        }
    }
}
