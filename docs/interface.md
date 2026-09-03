# 迷宫项目 — 接口契约（Interface Contract）

> 本文件是成员 A / B 之间**唯一接口依据**。
> 双方在各自分支开发，仅在接口契约、可视化集成、最终联调三处协同。
> 修改必须先在此处同步，再改代码；双方确认后方可定稿。
>
> **状态**：✅ 待确认（成员 A 已读）/ 待更新
> **最后更新**：2026-09-03
> **负责人**：成员 B

---

## 1. 总则

- 语言标准：**C++17**
- 字符约定：`'#'`=墙，`' '`=通路，`'S'`=起点，`'E'`=终点，`'.'`=路径高亮
- 坐标：**行优先** `(row, col)`，行/列均从 `0` 开始
- 行列含义：`row` 向下增，`col` 向右增；边界由 `inBounds` 统一判断
- 头文件按模块分包，置于 `include/` 下，与 `src/` 一一对应

---

## 2. 公共类型定义

位于 `include/core/Common.h`：

```cpp
#pragma once
#include <vector>
#include <utility>

// 坐标，(row, col)，行优先
using Pos  = std::pair<int, int>;

// 迷宫矩阵，grid[r][c] 为 '#' 墙 / ' ' 路 / 'S' 起点 / 'E' 终点
using Grid = std::vector<std::vector<char>>;
```

> 说明：为保证两成员接口一致，`Pos` 采用 `std::pair<int,int>`，**不定义独立结构体**（避免不同成员各自实现一套类型导致互不兼容）。

---

## 3. 公共判定约定

所有算法统一按以下规则判定格子是否可通行（由各函数内部自行实现，或抽成 `Common` 辅助函数）：

- `inBounds(r, c, grid)`：`0 <= r < rows && 0 <= c < cols`，越界返回 `false`。
- `isOpen(r, c, grid)`：在界内且 `grid[r][c] != '#'`。
- 起点 `S` / 终点 `E` 视为通路（可走）。
- 四方向邻接，**不支持斜向**（上下左右）。

---

## 4. 数据模型（成员 A 负责）

- **文件**：`include/model/Maze.h`、`src/model/Maze.cpp`
- 面向对象封装，内部持有 `Grid`，对外只暴露安全接口。
- **接口**：

```cpp
class Maze {
public:
    Maze(int h, int w);                    // 构造函数，全填 '#'
    int  height() const;
    int  width()  const;
    bool inBounds(int r, int c) const;
    bool isWall  (int r, int c) const;     // 越界视为墙
    void setCell (int r, int c, char ch);  // 越界忽略
    char cell    (int r, int c) const;     // 越界返回 '#'
    void print() const;                    // 终端字符渲染
};
```

- **协同约定**：`Maze` 只负责数据与打印；生成/求解算法一律通过 `const Grid&` 访问，不直接操作 `Maze` 内部，避免接口耦合。

---

## 5. 生成器（成员 B 负责）

### 5.1 DFS 生成器

- **文件**：`include/generator/DfsGenerator.h`、`src/generator/DfsGenerator.cpp`
- **接口**：

```cpp
class DfsGenerator {
public:
    DfsGenerator(unsigned w, unsigned h);  // w/h 必须为奇数
    void generate();                        // 生成完美迷宫（唯一通路、整体连通）
    const Grid& grid() const;
    Pos start() const;                     // 固定 (1,1)
    Pos goal()  const;                     // 固定 (h-2, w-2)
};
```

- 起点 `(1,1)`、终点 `(h-2, w-2)`，两者均保证为通路。
- 生成结果中不含 `'S'`/`'E'` 标记（仅墙与路），起点终点由成员 A 在主流程中标记。

---

## 6. 求解器（成员 B 负责）

所有求解函数：**输入为只读 `const Grid&`**，判据统一用 §3 规则；有解返回有序路径（含起点、终点），无解返回**空 `vector`**。

### 6.1 BFS / Dijkstra

- **文件**：`include/solver/Solver.h`、`src/solver/Solver.cpp`

```cpp
// BFS：无权图求最短路径（纯 `'#'` 障碍）
std::vector<Pos> bfsFind(const Grid& grid, Pos start, Pos goal);

// Dijkstra：加权图（为第 6 周动态/加权场景预留，当前位置代价=1）
std::vector<Pos> dijkstraFind(const Grid& grid, Pos start, Pos goal);
```

### 6.2 A*

- **文件**：`include/solver/AStar.h`、`src/solver/AStar.cpp`

```cpp
// 启发式 = 曼哈顿距离；f 相同时用行号打破平局
std::vector<Pos> aStarFind(const Grid& grid, Pos start, Pos goal);
```

### 6.3 一致性约定

- 三算法对同一迷宫、同一起终点，产生的**路径长度必须一致**（单测断言）。
- 返回值首元素为 `start`，末元素为 `goal`；目标不可达返回空。

---

## 7. 多终点（成员 B 负责）

- **文件**：`include/solver/MultiGoal.h`、`src/solver/MultiGoal.cpp`

```cpp
// 起点 + 全部终点，找出串起所有终点的最短路径（终点数 ≤ 8，状态压缩 DP）
// 按访问顺序拼接，去除重复端点；任一终点不可达则返回空
std::vector<Pos> multiGoalFind(const Grid& grid,
                               Pos start,
                               const std::vector<Pos>& goals);
```

---

## 8. 动态障碍物重规划（成员 B 负责）

- **文件**：`include/solver/Repath.h`、`src/solver/Repath.cpp`

```cpp
class RepathPlanner {
public:
    RepathPlanner(const Grid& grid, Pos s, Pos g);
    void setCell(int r, int c, char ch);            // 改动格子并触发重算
    bool hasPath() const;                            // 当前是否可达
    const std::vector<Pos>& path() const;            // 当前最优路径
    void setGoal(Pos g);                             // 支持运行中改终点（扩展）
};
```

- `setCell` 使路径变化后自动重算（事件触发式，内部调 A*）。
- 路径完全阻断时 `hasPath()==false`，`path()` 返回空。
- 恢复受阻格子后路径自动复原。

---

## 9. 核心工具（成员 A 负责）

- **文件**：`include/core/Utils.h`、`src/core/Utils.cpp`

```cpp
// 全局随机源，同种子可复现
std::mt19937& rng();
void setSeed(unsigned seed);

// 计时器，RAII，析构时打印耗时(ms)
class ScopedTimer { /* ... */ };

// 渲染：path 中格子用 '.' 高亮，起点/终点仍显 'S'/'E'
void printMaze(const Grid& grid,
               const std::vector<Pos>& path = {},
               Pos S = {-1,-1},
               Pos G = {-1,-1});

// 支持多个终点标记的渲染（第 5 周用）
void printMaze(const Grid& grid,
               const std::vector<Pos>& path,
               Pos S,
               const std::vector<Pos>& goals);
```

---

## 10. 文件 IO（成员 A 负责）

- **文件**：`include/core/MazeIO.h`、`src/core/MazeIO.cpp`
- 文本格式：首行 `h w`，之后逐行迷宫字符（`#`/` `）。

```cpp
bool saveMaze(const Grid& grid, const std::string& file);
bool loadMaze(const std::string& file, Grid& out);  // 行列不符返回 false
```

---

## 11. 单测约定

- 成员 B 自测文件：`tests/test_solver.cpp`（BFS/Dijkstra/A*、多终点、重规划）。
- 可用标准库 `assert` 断言，编译产出 `test_solver.exe`。
- 关键断言：算法长度一致、无解返回空、重规划后路径更新。

---

## 12. 变更记录

| 日期 | 版本 | 改动 | 确认方 |
|------|:----:|------|:------:|
| 2026-09-03 | v0.1 | 初稿，定义数据表示与全部模块接口 | — |