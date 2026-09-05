#pragma once

#include <vector>
#include "Point.h"

enum class CellType{
    Wall,
    Road,
    Start,
    End,
    Path,
    Visited
};
class Maze{
    private:
        int rows;
        int cols;
        
        std::vector<std::vector<CellType>> grid;
        
        Point start;
        Point end;

public:
    Maze(int rows, int cols);

    int getRows() const;
    int getcolsO() const;

    CellType getCell(int row,int col) const;
    void setCell(int row,int col,CellType type);

    bool isInside(int row,int col) const;
    bool isWalkable(int row,int col) const;

    Point getStart() const;
    Point getEnd() const;

    void setStart(Point point);
    void seteEnd(Point point);

    void clearPath();
};//用于保存二维迷宫
