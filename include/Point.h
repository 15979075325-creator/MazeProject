#pragma once
struct Point{
    int row;
    int col;

    bool operator== (const Point& other) const {
        return row == other.row &&
         col == other.col;
    }
};
 
