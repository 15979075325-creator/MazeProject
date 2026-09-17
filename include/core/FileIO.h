#pragma once

#include "core/Common.h"
#include <string>

bool saveMaze(const Grid& grid, const std::string& filename);

// Failure leaves grid unchanged. Rows preserve spaces; CRLF is accepted.
bool loadMaze(const std::string& filename, Grid& grid);
