// ============================================================
// demo_b.cpp —— 独立演示入口，不与 src/main.cpp 一起编译
// ------------------------------------------------------------
// 用途：使用共享 Utils/printMaze 验证
//       "DFS 生成 → BFS/A* 求解 → 终端打印" 闭环。
// 怎么跑：
//   g++ -std=c++17 -I include -o demo_b.exe
//       tests/demo_b.cpp src/generator/DfsGenerator.cpp
//       src/solver/Solver.cpp src/solver/AStar.cpp src/core/Utils.cpp
//   .\demo_b.exe
// 预期输出：BFS 与 A* 路径长度一致 + 一个 21x21 迷宫，
//           'S'/'E' 标记起终点，'.' 标出路径
// ============================================================

#include "generator/DfsGenerator.h"
#include "solver/Solver.h"
#include "solver/AStar.h"
#include "core/Utils.h"
#include <iostream>
#include <vector>

int main() {
    // 1. 生成 21x21 迷宫
    DfsGenerator gen(21, 21);
    gen.generate();

    // 2. BFS 与 A* 求解，对比路径长度
    auto path = bfsFind(gen.grid(), gen.start(), gen.goal());
    auto aPath = aStarFind(gen.grid(), gen.start(), gen.goal());
    if (path.empty()) {
        std::cout << "No path found.\n";
        return 1;
    }

    std::cout << "BFS path length = " << path.size() - 1 << "\n";
    std::cout << "A*  path length = " << aPath.size() - 1 << "\n";
    std::cout << (path.size() == aPath.size()
                    ? "lengths match"
                    : "LENGTH MISMATCH!")
              << "\n\n";

    // 3. 打印
    printMaze(gen.grid(), path, gen.start(), gen.goal());
    return 0;
}
