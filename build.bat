@echo off
cd /d "%~dp0"

g++ -std=c++17 ^
    -I include ^
    -o maze_app.exe ^
    src/main.cpp ^
    src/core/Utils.cpp ^
    src/core/FileIO.cpp ^
    src/model/Maze.cpp ^
    src/generator/DfsGenerator.cpp ^
    src/solver/Solver.cpp ^
    src/solver/AStar.cpp ^
    src/ui/ConsoleUI.cpp

if errorlevel 1 (
    echo Build failed
    pause
    exit /b 1
)

echo Build succeeded
echo.
maze_app.exe
set "mazeExitCode=%errorlevel%"
pause
exit /b %mazeExitCode%
