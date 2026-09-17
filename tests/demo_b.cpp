// ============================================================
// demo_b.cpp —— 成员B 本地自测入口（不属于正式工程，联调后删除）
// ------------------------------------------------------------
// 用途：成员A 的 Utils/printMaze 还没合并前，自己验证
//       "DFS 生成 → BFS 求解 → 终端打印" 闭环。
// 怎么跑：
//   g++ -std=c++17 -I include -o demo_b.exe
//       tests/demo_b.cpp src/generator/DfsGenerator.cpp src/solver/Solver.cpp
//   .\demo_b.exe
// 预期输出：一个 21x21 迷宫，'S'/'E' 标记起终点，'.' 标出路径
// ============================================================

#include "generator/DfsGenerator.h"
#include "solver/Solver.h"
#include <iostream>
#include <vector>

// ANSI 颜色：黄色高亮路径，起终点用亮黄
const char* YEL       = "\033[33m";    // 黄
const char* BOLD_YEL  = "\033[1;33m";  // 亮黄
const char* RESET     = "\033[0m";     // 复位

// 简易打印：路径格标 '.' 并显示为黄色，起终点标 S/E（一行根标签）
// TODO: 联调时删除本函数，改用成员A的 Utils::printMaze
static void printGrid(const Grid& g, const std::vector<Pos>& path, Pos s, Pos e) {
    Grid out = g;
    for (auto& row : out)
        for (auto& ch : row)
            if (ch == ' ') ch = ' ';           // 保持通路
    for (auto p : path) out[p.first][p.second] = '.';
    out[s.first][s.second] = 'S';
    out[e.first][e.second] = 'E';
    for (auto& row : out) {
        for (auto ch : row) {
            if (ch == '.')          std::cout << YEL      << ch << RESET;
            else if (ch == 'S' || ch == 'E')
                                    std::cout << BOLD_YEL << ch << RESET;
            else                    std::cout << ch;
        }
        std::cout << '\n';
    }
}

int main() {
    // 1. 生成 21x21 迷宫
    DfsGenerator gen(21, 21);
    gen.generate();

    // 2. BFS 求解
    auto path = bfsFind(gen.grid(), gen.start(), gen.goal());
    std::cout << "path length = " << path.size() << "\n\n";

    // 3. 打印
    printGrid(gen.grid(), path, gen.start(), gen.goal());
    return 0;
}
