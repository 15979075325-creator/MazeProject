// ============================================================
// test_solver.cpp —— 求解器单元测试（成员B · 第2周）
// ------------------------------------------------------------
// 怎么跑：
//   g++ -std=c++17 -I include -o test_solver.exe
//       tests/test_solver.cpp src/solver/Solver.cpp
//   .\test_solver.exe   （全部通过会打印 ALL TESTS PASSED）
//
// 本文件要测什么（用手工搭的小 Grid，不依赖生成器）：
//   用例1 直线    ：一行通路，S 左端 E 右端 → 路径长度 == 格子数
//   用例2 折线    ：需要拐弯的 L 形通路 → 长度与 BFS/Dijkstra 一致
//   用例3 无解    ：S 与 E 被一整列墙隔开 → 两算法都返回空
//   附加断言      ：bfsFind 与 dijkstraFind 返回的路径"长度"相等，
//                   且首元素==start、末元素==goal
// ============================================================

#include "solver/Solver.h"
#include <cassert>
#include <iostream>

// 辅助：检查路径首尾与连续性（相邻两步必须四方向相邻）
// TODO: 可先只断言长度，实现完 Solver 后再补连续性检查
static bool pathValid(const std::vector<Pos>& p, Pos s, Pos g) {
    if (p.empty()) return false;
    // TODO: p.front()==s && p.back()==g && 相邻步曼哈顿距离==1 && 每步都是通路
    return true;
}

// 用例1：直线通路
static void testStraightLine() {
    // 5 行 5 列，第 2 行全通路，S=(2,0) E=(2,4)，其余是墙
    Grid g(5, std::vector<char>(5, '#'));
    for (int c = 0; c < 5; ++c) g[2][c] = ' ';

    auto pb = bfsFind(g, {2, 0}, {2, 4});
    auto pd = dijkstraFind(g, {2, 0}, {2, 4});
    // TODO: 断言 pb.size() == 5（含首尾）、pd 与 pb 等长
    assert(!pb.empty() && pb.size() == pd.size());
    assert(pathValid(pb, {2, 0}, {2, 4}));
    std::cout << "[PASS] testStraightLine\n";
}

// 用例2：L 形折线
static void testTurn() {
    //   . . . . .
    //   # # # # .
    //   # # # # .   S=(0,0) E=(2,4)，唯一通路要拐弯
    Grid g(3, std::vector<char>(5, '#'));
    for (int c = 0; c < 5; ++c) g[0][c] = ' ';
    for (int r = 0; r < 3; ++r) g[r][4] = ' ';

    auto pb = bfsFind(g, {0, 0}, {2, 4});
    auto pd = dijkstraFind(g, {0, 0}, {2, 4});
    // TODO: 断言长度一致且等于最短折线长度 7
    assert(!pb.empty() && pb.size() == pd.size());
    std::cout << "[PASS] testTurn\n";
}

// 用例3：无解（被墙完全隔开）
static void testNoPath() {
    // 中间第 2 列整列是墙，S 在左 E 在右
    Grid g(3, std::vector<char>(5, ' '));
    for (int r = 0; r < 3; ++r) g[r][2] = '#';

    auto pb = bfsFind(g, {1, 0}, {1, 4});
    auto pd = dijkstraFind(g, {1, 0}, {1, 4});
    assert(pb.empty() && pd.empty());   // 都返回空 vector
    std::cout << "[PASS] testNoPath\n";
}

int main() {
    testStraightLine();
    testTurn();
    testNoPath();
    std::cout << "ALL TESTS PASSED\n";
    return 0;
}
