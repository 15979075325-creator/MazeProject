#pragma once

#include "core/Common.h"

class Maze {
private:
    int height_;
    int width_;
    Grid grid_;

public:
    Maze(int height, int width);
    explicit Maze(const Grid& grid);
    const Grid& grid() const;

    int height() const;
    int width() const;

    bool inBounds(int row, int col) const;
    bool isWall(int row, int col) const;

    char cell(int row, int col) const;
    void setCell(int row, int col, char value);

    void print() const;
};
