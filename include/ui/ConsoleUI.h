#pragma once

#include "core/Common.h"
#include "model/Maze.h"

#include <vector>

void clearScreen();

bool keyboardEdit(
    Maze& maze,
    std::vector<Pos>& path,
    Pos start,
    Pos end
);

void animatePath(
    const Grid& grid,
    const std::vector<Pos>& path,
    Pos start,
    Pos end
);
