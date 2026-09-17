#include "core/FileIO.h"

#include <fstream>
#include <limits>
#include <sstream>

static bool validCell(char cell) {
    return cell == '#' || cell == ' ' || cell == 'S' || cell == 'E' || cell == '.';
}

static bool validMaze(const Grid& grid) {
    if (grid.empty() || grid.front().empty()) return false;
    for (const auto& row : grid) {
        if (row.size() != grid.front().size()) return false;
        for (char cell : row) if (!validCell(cell)) return false;
    }
    return true;
}

bool saveMaze(const Grid& grid, const std::string& filename) {
    if (!validMaze(grid)) return false;
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    file << grid.size() << ' ' << grid.front().size() << '\n';
    for (const auto& row : grid) {
        file.write(row.data(), static_cast<std::streamsize>(row.size()));
        file.put('\n');
    }
    file.close();
    return !file.fail();
}

bool loadMaze(const std::string& filename, Grid& grid) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;
    std::string line;
    if (!std::getline(file, line)) return false;
    std::istringstream header(line);
    long long height = 0, width = 0;
    if (!(header >> height >> width) || height <= 0 || width <= 0 ||
        height > std::numeric_limits<int>::max() || width > std::numeric_limits<int>::max()) {
        return false;
    }
    header >> std::ws;
    if (!header.eof()) return false;
    Grid loaded;
    for (long long row = 0; row < height; ++row) {
        if (!std::getline(file, line)) return false;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.size() != static_cast<std::size_t>(width)) return false;
        for (char cell : line) if (!validCell(cell)) return false;
        loaded.emplace_back(line.begin(), line.end());
    }
    if (std::getline(file, line) || file.bad()) return false;
    grid = std::move(loaded);
    return true;
}
