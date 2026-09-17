// ============================================================
// AStar.cpp —— A* 实现（成员B · 第3周）
// ------------------------------------------------------------
// 本文件要实现 aStarFind：
//   1. 合法性检查：网格/起终点非法 → 返回空（同 bfsFind）
//   2. 数据结构：
//        gScore[rows][cols]    已走代价 g，起点 0，其余 INT_MAX/2
//        cameFrom[rows][cols]  前驱，回溯路径用
//        最小堆元素 (f, g, pos)：f = g + h；f 小者优先，
//        f 相同 g 大者优先（g 大 = 离终点更近，减少无效扩展）
//   3. 启发式 h = 曼哈顿距离 |r-gr| + |c-gc|
//      四方向、单位代价下可采纳（从不高估实际代价）→ 保证最优解
//   4. 主循环：
//        弹出 (f, g, cur)；g != gScore[cur] → 过期节点跳过
//        cur == goal → 提前终止（弹出时 gScore 已最优）
//        松弛四邻居：ng = g + 1；更小则更新 gScore/cameFrom 并入堆
//   5. 回溯 cameFrom → 反转 → 返回 start→goal 路径
// 辅助函数与 Solver.cpp 保持一致（static 各留一份，
// 后续如需共用可提去 core/ 公共头）
// ============================================================

#include "solver/AStar.h"

#include <algorithm>   // std::reverse
#include <climits>     // INT_MAX
#include <cstdlib>     // std::abs
#include <queue>       // std::priority_queue
#include <vector>

// 公共辅助函数（与 Solver.cpp 保持一致）
static bool validGrid(const Grid& grid) {
    if (grid.empty() || grid.front().empty()) return false;
    for (const auto& row : grid) {
        if (row.size() != grid.front().size()) return false;
    }
    return true;
}

static bool inBounds(int r, int c, const Grid& g) {//判断坐标是否存在
    if (g.empty()) return false;
    int rows = static_cast<int>(g.size());        // 行数
    int cols = static_cast<int>(g[0].size());     // 列数（每行等长）
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

static bool isOpen(const Grid& g, int r, int c) {//判断能不能走这个格子
    // 先保证坐标合法，再判断不是墙；' '、'S'、'E' 都算通路
    return inBounds(r, c, g) && g[r][c] != '#';
}

// 四方向：上、下、左、右（行优先）
static const int DR[4] = {-1, 1, 0, 0};
static const int DC[4] = {0, 0, -1, 1};

// 曼哈顿启发式：四方向单位代价下可采纳（不高估），保证结果最优
static int heuristic(Pos a, Pos b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

// 堆元素：f 为优先值，g 用于打破平局
struct Node {
    int f;
    int g;
    Pos pos;
};

// 比较器：a 优先级低于 b 时返回 true
//   f 不同：f 大的优先级低（最小堆效果）
//   f 相同：g 小的优先级低（g 大 = 已走更多 = 离终点更近，先扩展）
struct LowerPriority {
    bool operator()(const Node& a, const Node& b) const {
        if (a.f != b.f) return a.f > b.f;
        return a.g < b.g;
    }
};

// A* 求最短路径（网格所有边权均为 1，结果与 BFS/Dijkstra 等长）
std::vector<Pos> aStarFind(const Grid& grid, Pos start, Pos goal) {
    // 1. 合法性检查同 bfsFind
    if (!validGrid(grid) || !isOpen(grid, start.first, start.second)
                    || !isOpen(grid, goal.first, goal.second)) {
        return {};
    }

    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());

    // 2. gScore / cameFrom 初始化
    const int INF = INT_MAX / 2;
    std::vector<std::vector<int>> gScore(rows, std::vector<int>(cols, INF));
    std::vector<std::vector<Pos>> cameFrom(rows, std::vector<Pos>(cols, {-1, -1}));

    // 3. 最小堆：起点入堆，f = 0 + h(start)
    std::priority_queue<Node, std::vector<Node>, LowerPriority> pq;
    gScore[start.first][start.second] = 0;
    pq.push({heuristic(start, goal), 0, start});

    bool found = false;
    while (!pq.empty()) {
        Node node = pq.top();
        pq.pop();

        // 过期节点：gScore 已被更小的值更新过
        if (node.g != gScore[node.pos.first][node.pos.second]) continue;

        // 弹出 goal 时 gScore[goal] 已最优，可提前终止
        if (node.pos == goal) { found = true; break; }

        // 松弛四个方向的邻居
        for (int i = 0; i < 4; ++i) {
            int nr = node.pos.first + DR[i];
            int nc = node.pos.second + DC[i];
            if (!isOpen(grid, nr, nc)) continue;
            int ng = node.g + 1;      // 网格内每步权值为 1
            if (ng < gScore[nr][nc]) {
                gScore[nr][nc] = ng;
                cameFrom[nr][nc] = node.pos;
                int nf = ng + heuristic({nr, nc}, goal);   // f = g + h
                pq.push({nf, ng, {nr, nc}});
            }
        }
    }

    if (!found) return {};   // 堆耗尽仍未到终点 → 无解

    // 4. 回溯 cameFrom 返回路径
    std::vector<Pos> path;
    Pos cur = goal;
    while (cur != start) {
        path.push_back(cur);
        cur = cameFrom[cur.first][cur.second];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}
