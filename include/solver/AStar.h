#pragma once
// ============================================================
// AStar.h —— A* 路径求解器（成员B · 第3周）
// ------------------------------------------------------------
// 功能：在 Grid 迷宫中找 start → goal 的最短路径
//   - aStarFind：优先队列(最小堆) + 曼哈顿启发式 f = g + h
//     启发式在四方向移动下可采纳（不高估实际代价），保证最优
//     相比 BFS/Dijkstra 更快朝终点收拢，扩展节点更少
// 约定（与 Solver.h 一致，见 docs/interface.md 求解器章节）：
//   - 坐标行优先；四方向移动，不斜走；'#' 不可走，'S'/'E' 视为通路
//   - 有解：返回路径 vector，首元素=start、末元素=goal
//   - 无解：返回空 vector（不抛异常）
//   - 三算法（A*/BFS/Dijkstra）对同一输入返回的路径"长度"必须一致
// ============================================================

#include "core/Common.h"
#include <vector>

// A* 求最短路径（当前所有可走格代价相同，结果应与 BFS/Dijkstra 等长）
// 实现：gScore 数组记录已走代价，cameFrom 记录前驱回溯路径
//       最小堆元素 (f, g, pos)；f 相同时 g 大者优先（更接近终点）
std::vector<Pos> aStarFind(const Grid& grid, Pos start, Pos goal);
