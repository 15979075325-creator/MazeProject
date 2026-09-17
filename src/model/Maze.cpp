#include "../../include/model/Maze.h"

#include <iostream>
#include <stdexcept>
#include <limits>

Maze::Maze(int height, int width)
    : height_(height),
      width_(width),
      grid_() {
    if (height <= 0 || width <= 0) {
        throw std::invalid_argument("Maze dimensions must be positive");
    }
    grid_.assign(height, std::vector<char>(width, '#'));
}

Maze::Maze(const Grid& grid) : height_(0), width_(0), grid_() {
    if (grid.empty() || grid.front().empty() ||
        grid.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
        grid.front().size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::invalid_argument("Maze grid must be nonempty with valid dimensions");
    }
    for (const auto& row : grid) {
        if (row.size() != grid.front().size()) {
            throw std::invalid_argument("Maze rows must have equal widths");
        }
    }
    height_ = static_cast<int>(grid.size());
    width_ = static_cast<int>(grid.front().size());
    grid_ = grid;
}

const Grid& Maze::grid() const {
    return grid_;
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
