@echo off
REM ============================================================
REM build.bat - build test_solver.exe and demo_b.exe (member B wk2)
REM ------------------------------------------------------------
REM Usage : run `build.bat` in project root (or double-click)
REM Output: test_solver.exe, demo_b.exe (in current directory)
REM Prereq: g++ must be in PATH (install MinGW and set env)
REM ============================================================

REM ---------- 1. build test_solver.exe ----------
REM -std=c++17                 use C++17 standard
REM -I include                 find headers Solver.h / Common.h
REM -o test_solver.exe         output executable name
REM tests/test_solver.cpp      test functions + main()
REM src/solver/Solver.cpp      implementation of bfsFind/dijkstraFind
g++ -std=c++17 -I include -o test_solver.exe ^
    tests/test_solver.cpp ^
    src/solver/Solver.cpp

if errorlevel 1 (
    echo [ERROR] test_solver.exe BUILD FAILED.
    exit /b 1
) else (
    echo [OK] built test_solver.exe
)

REM ---------- 2. build demo_b.exe ----------
g++ -std=c++17 -I include -o demo_b.exe ^
    tests/demo_b.cpp ^
    src/generator/DfsGenerator.cpp ^
    src/solver/Solver.cpp

if errorlevel 1 (
    echo [ERROR] demo_b.exe BUILD FAILED.
    exit /b 1
) else (
    echo [OK] built demo_b.exe
)

echo BUILD OK, created test_solver.exe and demo_b.exe