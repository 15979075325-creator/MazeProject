# MazeProject 接口约定

## 一、字符约定

| 字符 | 含义 |
|---|---|
| `#` | 墙 |
| 空格 | 通路 |
| `S` | 起点 |
| `E` | 终点 |
| `.` | 最终路径 |

示例：

```text
#########
#S    E#
#########
```

## 二、坐标约定

坐标格式为：

```cpp
(row, col)
```

其中：

- `row` 表示行，从 0 开始
- `col` 表示列，从 0 开始
- `row` 增大表示向下移动
- `col` 增大表示向右移动

例如：

```cpp
Pos start{1, 1};
```

表示第 1 行、第 1 列。

## 三、公共类型

统一使用以下类型：

```cpp
#include <utility>
#include <vector>

using Pos = std::pair<int, int>;
using Grid = std::vector<std::vector<char>>;
```

## 四、迷宫数据模型

迷宫类文件：

```text
include/model/Maze.h
src/model/Maze.cpp
```

接口：

```cpp
class Maze {
public:
    Maze(int height, int width);

    int height() const;
    int width() const;

    bool inBounds(int row, int col) const;
    bool isWall(int row, int col) const;

    char cell(int row, int col) const;
    void setCell(int row, int col, char value);

    void print() const;
};
```

要求：

- 新建迷宫时，所有格子默认为 `'#'`
- 越界访问不能导致程序崩溃
- `isWall()` 越界时返回 `true`
- `cell()` 越界时返回 `'#'`
- `setCell()` 越界时不进行修改

## 五、起点和终点

默认起点：

```cpp
Pos start{1, 1};
```

默认终点：

```cpp
Pos goal{height - 2, width - 2};
```

迷宫的高度和宽度最好使用奇数，例如：

```text
11 × 11
21 × 21
41 × 41
101 × 101
```

## 六、迷宫生成器

生成器文件：

```text
include/generator/DfsGenerator.h
src/generator/DfsGenerator.cpp
```

接口：

```cpp
class DfsGenerator {
public:
    DfsGenerator(unsigned width, unsigned height);

    void generate();

    const Grid& grid() const;

    Pos start() const;
    Pos goal() const;
};
```

要求：

- 初始迷宫全部为 `'#'`
- 使用 DFS 递归回溯或迭代回溯
- 生成的道路使用空格 `' '`
- 起点固定为 `(1, 1)`
- 终点固定为 `(height - 2, width - 2)`
- 起点和终点必须可到达
- 宽度和高度最好为奇数

## 七、求解器

求解器文件：

```text
include/solver/Solver.h
src/solver/Solver.cpp
```

### BFS

```cpp
std::vector<Pos> bfsFind(
    const Grid& grid,
    Pos start,
    Pos goal
);
```

### Dijkstra

```cpp
std::vector<Pos> dijkstraFind(
    const Grid& grid,
    Pos start,
    Pos goal
);
```

### A*

```cpp
std::vector<Pos> aStarFind(
    const Grid& grid,
    Pos start,
    Pos goal
);
```

要求：

- `'#'` 不能通行
- 空格、`S`、`E` 可以通行
- 找到路径时，返回从起点到终点的坐标序列
- 找不到路径时，返回空的 `std::vector<Pos>`
- BFS、Dijkstra、A* 在无权迷宫中的路径长度应相同

无解时：

```cpp
return {};
```

## 八、路径格式

路径示例：

```cpp
{
    {1, 1},
    {1, 2},
    {1, 3},
    {2, 3},
    {3, 3}
}
```

路径应包含：

- 起点
- 中间经过的格子
- 终点

路径显示时：

- 起点仍显示为 `S`
- 终点仍显示为 `E`
- 其他路径位置显示为 `.`

## 九、核心工具

核心工具文件：

```text
include/core/Utils.h
src/core/Utils.cpp
```

随机数接口：

```cpp
std::mt19937& rng();

void setSeed(unsigned int seed);
```

计时器接口：

```cpp
class ScopedTimer {
public:
    explicit ScopedTimer(const char* name);
    ~ScopedTimer();
};
```

## 十、文件保存格式

迷宫文件使用普通文本格式。

第一行保存高度和宽度：

```text
9 9
```

后面每一行保存迷宫内容：

```text
#########
#S    E#
#########
#########
#########
#########
#########
#########
#########
```

保存函数：

```cpp
bool saveMaze(
    const Grid& grid,
    const std::string& filename
);
```

读取函数：

```cpp
bool loadMaze(
    const std::string& filename,
    Grid& grid
);
```

如果文件不存在、尺寸错误或内容错误，返回：

```cpp
false
```

成功返回：

```cpp
true
```

## 十一、文件命名规范

头文件使用：

```text
.h
```

源文件使用：

```text
.cpp
```

类名使用大写开头：

```cpp
Maze
DfsGenerator
Solver
```

函数名使用小写加驼峰：

```cpp
getPath
isWall
setCell
findPath
```

变量名使用小写加下划线或驼峰：

```cpp
grid
startPos
goalPos
```

## 十二、编译要求

Windows 使用：

```text
build.bat
```

Linux 或 macOS 使用：

```text
build.sh
```

Windows 编译命令：

```bat
g++ -std=c++17 ^
    -I include ^
    -o maze_app.exe ^
    src/main.cpp ^
    src/model/Maze.cpp ^
    src/core/Utils.cpp
```

后续新增 `.cpp` 文件时，需要将文件路径加入编译命令。

## 十三、分支约定

主分支：

```text
main
```

成员 A 分支：

```text
member-a
```

成员 B 分支：

```text
member-b
```

开发流程：

```text
在个人分支编写代码
        ↓
本地编译和测试
        ↓
提交代码
        ↓
创建 Pull Request
        ↓
合并到 main
```
