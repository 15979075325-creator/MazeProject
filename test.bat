@echo off
cd /d "%~dp0"

g++ -std=c++17 -I include ^
    tests/test_solver.cpp ^
    src/solver/Solver.cpp ^
    -o test_solver.exe

if errorlevel 1 (
    echo Test build failed
    pause
    exit /b 1
)

test_solver.exe
pause
