#include "../include/Maze.h"
#include <iostream>

Maze::Maze(int rows,int cols)
    :rows(rows),
    cols(cols),
    grid(rows,std::vector<CellType>(
        cols,CellType::Wall)),
    start{1,1},
    end{rows - 2,cols - 2}{
    if(isInside(start.row,start.col)){
        grid[start.row][start.col] = CellType::Start;
    }

    if(isInside(end.row,end.col)){
        grid[end.row][end.col] = CellType::End;
     }
    }
    int Maze::getRows() const {
        return rows;
    }
    int Maze::getCols() const {
        return cols;
    }

    CellType Maze::getCell(int row,int col)const{
        return grid[row][col];
    }
    
    void Maze::setCell(int row,int col,CellType type){
        if(isInside(row,col)){
            grid[row][col] = type;
        }
    }
    
    bool Maze::isInside(int row,int col) const{
       return row>=0 &&
              row<rows &&
              col>=0&&
              col<cols;
    }

    bool Maze::isWalkable(int row,int col)const{
        if(!isInside(row,col)){
            return false;
        }

        return grid[row][col]!= CellType::Wall;
    }
    
    Point Maze::getStart()const{
        return start;
    }
    Point Maze::getEnd() const{
        return end;
    }
    void Maze::setStart(Point point){
        if(isInside(point.row,point.col)){
            grid[start.row][start.col] = CellType::Road;

            start = point;
            grid[start.row][start.col] = CellType::Start;
        }
    }

    void Maze::setEnd(Point point){
        if(isInside(point.row,point.col)){
            grid[end.row][end.col] = CellType::Road;

            end = point;
            grid[end.row][end.col] = CellType::End;
        }
    }

    void Maze::clearPath(){
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                if(grid[i][j]==CellType::Path||
                   grid[i][j]==CellType::Visited){
                   grid[i][j]= CellType::Road;
                }
            }
        }
        grid[start.row][start.col] = CellType::Start;
        grid[end.row][end.col] = CellType::End;
    }
    void Maze::print() const{
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                switch(grid[i][j]){
                    case CellType::Wall:
                        std::cout << "##";
                        break;
                    case CellType::Road:
                        std::cout << "  ";
                        break;
                    case CellType::Start:
                        std::cout << "ST";
                        break;
                    case CellType::End:
                        std::cout << "ED";
                        break;
                    case CellType::Path:
                        std::cout << "..";
                        break;
                    case CellType::Visited:
                        std::cout << "++";
                        break;
                }
            }
            std::cout << '\n';
        }
    }
    

