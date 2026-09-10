#include "../../include/model/Maze.h"

#include <iostream>

Maze::Maze(int height, int width)
    : height_(height),
      width_(width),
      grid_(height, std::vector<char>(width, '#')) {
}

int Maze::height() const {
    return height_;
}

int Maze::width() const {
    return width_;
}

bool Maze::inBounds(int row, int col) const {
    return row >= 0 &&
           row < height_ &&
           col >= 0 &&
           col < width_;
}

bool Maze::isWall(int row, int col) const {
    if (!inBounds(row, col)) {
        return true;
    }

    return grid_[row][col] == '#';
}

char Maze::cell(int row, int col) const {
    if (!inBounds(row, col)) {
        return '#';
    }

    return grid_[row][col];
}

void Maze::setCell(int row, int col, char value) {
    if (inBounds(row, col)) {
        grid_[row][col] = value;
    }
}

void Maze::print() const {
    for (int row = 0; row < height_; row++) {
        for (int col = 0; col < width_; col++) {
            std::cout << grid_[row][col];
        }

        std::cout << '\n';
    }
}
