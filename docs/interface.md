# 接口契约文档 (docs/interface.md)

> 编写人：成员B ｜ 审核人：成员A ｜ 第 1 周
> 状态：草案，双方确认后定稿
> 目的：动手编码前锁定全项目统一的数据结构、坐标与函数签名，避免集成冲突。

---

## 1. 迷宫数据表示

全项目**统一**使用二维 `char` 矩阵表示迷宫，称为 `Grid`。

```cpp
using Grid = std::vector<std::vector<char>>;
```

- `grid[row][col]`：`row` 为行号（第 1 维，纵坐标），`col` 为列号（第 2 维，横坐标）。
- 矩阵下标从 0 开始。

### 1.1 字符约定

| 字符 | 含义 |
|------|------|
| `'#'` | 墙 |
| `' '` | 路（可通行） |
| `'S'` | 起点 |
| `'E'` | 终点 |

> 求解路径在**可视化阶段**可用 `.` 高亮，但 `Grid` 本身只存四种基础字符；路径由求解器单独返回。

---

## 2. 坐标类型

统一用 `std::pair<int,int>` 表示坐标，命名为 `Pos`。

```cpp
using Pos = std::pair<int, int>;
// Pos.first  = row（行）
// Pos.second = col（列）
```

### 2.1 越界判断约定
- 越界或 `grid[r][c] == '#'` 均视为「不可通行」。
- 起点、终点必须落在可通行格（路）上，且应在迷宫边界内。

---

## 3. 模块接口

### 3.1 迷宫生成器

输入：宽度、高度（**需为奇数**，偶数由生成器自动调整为不小于的奇数）。
输出：`Grid` 迷宫矩阵。

```cpp
// 递归回溯（DFS）生成器
class DfsGenerator {
public:
    DfsGenerator(unsigned width, unsigned height);   // width/height 为奇数
    void generate();                                  // 生成完美迷宫
    const Grid& grid() const;                         // 取迷宫矩阵
    Pos start() const;   // 起点，固定为 (1,1)
    Pos goal()  const;   // 终点，固定为 (height-2, width-2)
};
```

复杂度（环路）调节放后续增加：`generateWithComplexity(float)`，`0~1` 越大环路越多。

### 3.2 求解器

输入：`Grid`、起点、终点。
输出：**含起点、终点的完整路径**；无解返回空 `vector`。

```cpp
namespace solver {
    // 返回路径（含起终点）；无解返回空 vector
    std::vector<Pos> bfsFind(const Grid& grid, Pos start, Pos goal);       // BFS（无权最短路）
    std::vector<Pos> dijkstraFind(const Grid& grid, Pos start, Pos goal);  // Dijkstra（加权）
    std::vector<Pos> aStarFind(const Grid& grid, Pos start, Pos goal);     // A*（曼哈顿启发式）
} // namespace solver
```

多终点接口（后续）：输入起点 + 终点列表，输出依次经过所有终点的连续路径。

```cpp
namespace solver {
    std::vector<Pos> multiGoalFind(const Grid& grid, const Pos& start,
                                   const std::vector<Pos>& goals);
} // namespace solver
```

动态障碍 / 重规划接口（后续）：

```cpp
class RepathPlanner {
public:
    RepathPlanner(const Grid& grid, Pos start, Pos goal);
    void setCell(int r, int c, char ch);          // 改动某格，自动重算
    bool hasPath() const;
    const std::vector<Pos>& path() const;
};
```

---

## 4. 与 Maze 类的关系

成员A 的 `Maze` 类是面向「打印/编辑」的封装；它内部持有 `Grid`，对外提供：

```cpp
class Maze {
public:
    const Grid& grid() const;          // 取内部矩阵（统一格式）
    void setGrid(const Grid& g);       // 从统一格式装载
};
```

- **约定**：生成器、求解器只认 `Grid` + `Pos`；`Maze` 是使用层包装，二者通过 `Grid` 互相转换，不直接访问对方私有成员。

---

## 5. 字符/坐标核对清单（提交前自查）

- [ ] 所有坐标均为 `(row, col)`，与 `grid[r][c]` 对应，未混淆行列。
- [ ] `Grid` 只用 `'#'`/`' '`/`'S'`/`'E'`，路径用单独 `vector<Pos>` 返回。
- [ ] 越界或 `'#'` 视为不可通行。
- [ ] 生成器尺寸为奇数；起终点固定在有效路格。
- [ ] 求解器返回空 `vector` 表示无解（不抛异常）。

---

## 6. 确认记录

| 成员 | 确认状态 | 日期 |
|------|:------:|:----:|
| 成员A | 待确认 | - |
| 成员B | 已起草 | - |