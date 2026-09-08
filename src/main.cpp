#include<iostream>
#include "../include/Maze.h"

int main(){
    Maze maze(11,11);

    std::cout << "Rows: "
              << maze.getRows() 
              << '\n';

    std::cout << "Cols: "
              << maze.getCols()
              << '\n';

    Point start = maze.getStart();
    Point end = maze.getEnd();

    std::cout << "Start: "
              << start.row <<","
              << start.col <<'\n';

    std::cout << "End: "
              << end.row <<","
              << end.col <<'\n';

    std::cout << "\nMaze:\n";

    maze.print();

    return 0;
}
