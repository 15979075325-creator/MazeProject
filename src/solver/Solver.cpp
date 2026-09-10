// ============================================================
// Solver.cpp —— BFS / Dijkstra 实现（成员B · 第2周）
// ------------------------------------------------------------
// 本文件要实现：
//   1. 公共辅助（本文件内 static 即可，第3周 A* 也要用可考虑提去 Common）
//        inBounds(r, c, grid)：0 <= r < rows && 0 <= c < cols
//        isOpen(grid, r, c)  ：界内且 grid[r][c] != '#'
//   2. bfsFind：
//        - 起点/终点越界或起点是墙 → 返回空
//        - queue 逐层扩展，visited 防重，parent 记录前驱
//        - 到达 goal 后沿 parent 回溯，反转得到 start→goal 路径
//   3. dijkstraFind：
//        - dist 全部初始化为 INT_MAX/2，dist[start]=0
//        - 最小堆元素 (d, pos)；弹出时 d > dist[pos] 则跳过（过期节点）
//        - 松弛邻居：nd = d + 1，更小则更新 dist/parent 并入堆
//        - 同样回溯 parent
// 提醒：两函数返回的路径长度必须一致，test_solver 会断言
// ============================================================

#include "solver/Solver.h"

#include <algorithm>   // std::reverse
#include <climits>     // INT_MAX
#include <queue>       // std::queue / std::priority_queue
#include <vector>

// 公共辅助函数
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

// BFS 求最短路径
std::vector<Pos> bfsFind(const Grid& grid, Pos start, Pos goal) {
    // 1. 合法性检查：起点/终点越界或是墙 → 返回空
    if (grid.empty() || !isOpen(grid, start.first, start.second)
                    || !isOpen(grid, goal.first, goal.second)) {
        return {};
    }

    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());

    // 2. visited 防重、parent 记录前驱
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::vector<std::vector<Pos>> parent(rows, std::vector<Pos>(cols, {-1, -1}));

    std::queue<Pos> q;
    q.push(start);
    visited[start.first][start.second] = true;   // 入队时标记，防重复入队

    bool found = false;
    while (!q.empty()) {
        Pos cur = q.front();
        q.pop();

        if (cur == goal) { found = true; break; }   // 到达终点

        // 3. 向四个方向扩展能走的邻居
        for (int d = 0; d < 4; ++d) {
            int nr = cur.first + DR[d];
            int nc = cur.second + DC[d];
            if (isOpen(grid, nr, nc) && !visited[nr][nc]) {
                visited[nr][nc] = true;
                parent[nr][nc] = cur;
                q.push({nr, nc});
            }
        }
    }

    if (!found) return {};   // 队列耗尽仍未到终点 → 无解

    // 4. 从 goal 沿 parent 回溯到 start，再反转成 start→goal
    std::vector<Pos> path;
    Pos cur = goal;
    while (cur != start) {
        path.push_back(cur);
        cur = parent[cur.first][cur.second];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

// Dijkstra 求最短路径（网格所有边权均为 1，结果与 BFS 一致）
std::vector<Pos> dijkstraFind(const Grid& grid, Pos start, Pos goal) {
    // 1. 合法性检查同上
    if (grid.empty() || !isOpen(grid, start.first, start.second)
                    || !isOpen(grid, goal.first, goal.second)) {
        return {};
    }

    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());

    // 2. dist 初始化为大值 + parent 记录前驱
    const int INF = INT_MAX / 2;
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, INF));
    std::vector<std::vector<Pos>> parent(rows, std::vector<Pos>(cols, {-1, -1}));

    // 最小堆元素 (d, pos)：优先取距离最小的
    using Node = std::pair<int, Pos>;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    dist[start.first][start.second] = 0;
    pq.push({0, start});

    bool found = false;
    while (!pq.empty()) {
        int d = pq.top().first;
        Pos cur = pq.top().second;
        pq.pop();

        // 过期节点（dist 已被更小值更新）跳过
        if (d != dist[cur.first][cur.second]) continue;

        // 3. 到达终点可提前终止
        if (cur == goal) { found = true; break; }

        // 松弛四个方向的邻居
        for (int i = 0; i < 4; ++i) {
            int nr = cur.first + DR[i];
            int nc = cur.second + DC[i];
            if (!isOpen(grid, nr, nc)) continue;
            int nd = d + 1;      // 网格内每步权值为 1
            if (nd < dist[nr][nc]) {
                dist[nr][nc] = nd;
                parent[nr][nc] = cur;
                pq.push({nd, {nr, nc}});
            }
        }
    }

    if (!found) return {};   // 堆耗尽仍未到终点 → 无解

    // 4. 回溯 parent 返回路径
    std::vector<Pos> path;
    Pos cur = goal;
    while (cur != start) {
        path.push_back(cur);
        cur = parent[cur.first][cur.second];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}
