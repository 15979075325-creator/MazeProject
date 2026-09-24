@echo off
cd /d "%~dp0"

g++ -std=c++17 ^
    -I include ^
    -mwindows ^
    -o maze_gui.exe ^
    src/gui_main.cpp ^
    src/core/Utils.cpp ^
    src/core/FileIO.cpp ^
    src/generator/DfsGenerator.cpp ^
    src/solver/Solver.cpp ^
    src/solver/AStar.cpp ^
    -lgdi32 ^
    -lcomdlg32

if errorlevel 1 (
    echo GUI build failed
    pause
    exit /b 1
)

echo GUI build succeeded
start "" maze_gui.exe
exit /b 0
