@echo off
cd /d "%~dp0"

g++ -std=c++17 -I include ^
    tests/test_solver.cpp ^
    src/solver/Solver.cpp ^
    src/solver/AStar.cpp ^
    -o test_solver.exe

if errorlevel 1 (
    echo Test build failed
    pause
    exit /b 1
)

test_solver.exe
if errorlevel 1 goto failed

g++ -std=c++17 -I include ^
    tests/test_project.cpp ^
    src/core/Utils.cpp ^
    src/core/FileIO.cpp ^
    src/model/Maze.cpp ^
    src/generator/DfsGenerator.cpp ^
    src/solver/Solver.cpp ^
    -o test_project.exe
if errorlevel 1 goto failed

test_project.exe
if errorlevel 1 goto failed
echo All tests succeeded
pause
exit /b 0

:failed
echo Tests failed
pause
exit /b 1
