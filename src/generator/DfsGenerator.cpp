// ============================================================
// DfsGenerator.cpp —— DFS 迷宫生成器实现（成员B · 第2周）
// ------------------------------------------------------------
// 本文件要实现：
//   1. 构造函数：校验 w/h 为不小于 3 的奇数，初始化全 '#' 的 Grid
//   2. carve(r,c)：递归回溯挖路
//        - 当前格改成 ' '
//        - 四个方向（上下左右、步长2）放进 vector 后用 std::shuffle 打乱
//        - 若隔一跳的目标格在界内且仍是墙：
//            打通中间格（置 ' '）→ 递归 carve 目标格
//   3. generate()：从 (1,1) 启动 carve
// 提示：shuffle 需要随机数引擎；第2周成员A的 Utils::rng() 可能还没合并，
//       可先用局部 static std::mt19937{std::random_device{}()} 顶上，
//       联调时再换成 rng()（同种子可复现）
// ============================================================

#include "generator/DfsGenerator.h"
#include "core/Utils.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

// 局部随机源：用"当前时间 ^ 设备随机数"做种子，保证每次启动都不一样
// （注意：某些 MinGW 下 random_device 每次都返回相同值，必须掺入时间）
static std::mt19937& rng() {
    using namespace std::chrono;
    auto seed = static_cast<unsigned>(
        high_resolution_clock::now().time_since_epoch().count()) ^
        std::random_device{}();
    static std::mt19937 e{seed};
    return e;
}

// 构造函数
DfsGenerator::DfsGenerator(unsigned w, unsigned h)
    : _grid(), _w(w), _h(h) {
    // 1. 参数校验：w、h 必须是 >= 3 的奇数
    if (w < 3 || h < 3 || w % 2 == 0 || h % 2 == 0) {
        throw std::invalid_argument("DfsGenerator: w/h 必须是不小于 3 的奇数");
    }
    // 2. 初始化为 h 行 w 列、全部填 '#'
    _grid.assign(h, std::vector<char>(w, '#'));
}

// 递归挖路
void DfsGenerator::carve(int r, int c) {
    // 1. 当前格挖成通路
    _grid[r][c] = ' ';

    // 2. 四方向偏移（步长2）并随机打乱
    std::vector<std::pair<int, int>> dirs = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
    std::shuffle(dirs.begin(), dirs.end(), rng());

    // 3. 遍历方向：打通"隔一跳"的墙并递归
    int rows = static_cast<int>(_grid.size());
    int cols = static_cast<int>(_grid[0].size());
    for (const auto& d : dirs) {
        int dr = d.first;
        int dc = d.second;
        int nr = r + dr;
        int nc = c + dc;
        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols
            && _grid[nr][nc] == '#') {
            _grid[nr][nc] = ' ';                    // 目标格
            _grid[r + dr / 2][c + dc / 2] = ' ';    // 打通中间墙
            carve(nr, nc);                          // 递归
        }
    }
}

// 生成迷宫
void DfsGenerator::generate() {
    carve(1, 1);
}
