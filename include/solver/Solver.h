#pragma once
// ============================================================
// Solver.h —— BFS / Dijkstra 路径求解器（成员B · 第2周）
// ------------------------------------------------------------
// 功能：在 Grid 迷宫中找 start → goal 的最短路径
//   - bfsFind：队列 + visited + parent 回溯（无权图最短路）
//   - dijkstraFind：优先队列(最小堆) + dist 松弛 + 过期节点跳过
//     （当前位置代价=1，为第6周加权场景预留接口）
// 约定（docs/interface.md §6）：
//   - 坐标行优先；四方向移动，不斜走；'#' 不可走，'S'/'E' 视为通路
//   - 有解：返回路径 vector，首元素=start、末元素=goal
//   - 无解：返回空 vector（不抛异常）
//   - 两算法对同一输入返回的路径"长度"必须一致
// ============================================================

#include "core/Common.h"
#include <vector>

// BFS 求最短路径
// TODO: queue<Pos> + visited 二维bool + parent 二维Pos，弹出时判 goal，最后回溯
std::vector<Pos> bfsFind(const Grid& grid, Pos start, Pos goal);

// Dijkstra 求最短路径（当前所有可走格代价相同，结果应与 BFS 等长）
// TODO: priority_queue<pair<int,Pos>, vector<...>, greater<...>> 最小堆
//       dist 数组用大数初始化，弹出时若 dist 过期则跳过（continue）
std::vector<Pos> dijkstraFind(const Grid& grid, Pos start, Pos goal);
