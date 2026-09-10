#pragma once

#include <vector>

class Maze {
private:
    int height_;
    int width_;
    std::vector<std::vector<char>> grid_;

public:
    Maze(int height, int width);

    int height() const;
    int width() const;

    bool inBounds(int row, int col) const;
    bool isWall(int row, int col) const;

    char cell(int row, int col) const;
    void setCell(int row, int col, char value);

    void print() const;
};
