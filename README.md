# MazeProject

[![C++ Build and Tests](https://github.com/15979075325-creator/MazeProject/actions/workflows/tests.yml/badge.svg)](https://github.com/15979075325-creator/MazeProject/actions/workflows/tests.yml)

基于 C++17 的命令行迷宫生成、编辑、求解与可视化项目。

## 环境与运行

Windows 需要安装 `g++`，并确保可从 `PATH` 调用。

- 双击 `build.bat`：编译并运行主程序。
- 双击 `test.bat`：编译并运行全部自动测试。
- 测试失败时，脚本返回非零退出码。
- GitHub Actions 会在 main、A、B 分支推送时自动编译并测试。

| 字符 | 含义 |
|---|---|
| `#` | 墙 |
| 空格 | 通路 |
| `S` | 起点（Start） |
| `E` | 终点（End） |
| `.` | 求解得到的路径 |
| `@` | 键盘编辑模式中的光标 |

## 主菜单

```text
--- Maze setup ---
1. Generate a new maze (custom size)
2. Edit a cell
3. Set start and end
10. Keyboard editor

--- Pathfinding ---
4. Find path with BFS
5. Find path with Dijkstra
6. Find path with A*
7. Compare BFS, Dijkstra and A*
11. Animate the current path

--- Files ---
8. Save maze
9. Load maze

0. Exit
```

### 生成迷宫

选项 `1` 输入 `行数 列数 种子`。行数和列数必须是 3～51 的奇数，例如：

```text
21 21 12345
```

相同尺寸和种子会生成完全相同的迷宫。种子输入 `0` 时随机生成，程序会显示本次实际种子，便于之后复现。

### 编辑迷宫

- 选项 `2`：输入 `行 列 类型`，其中 `0` 是通路，`1` 是墙。
- 选项 `3`：输入 `起点行 起点列 终点行 终点列`。
- 选项 `10`：进入键盘编辑模式。
  - `W/A/S/D` 或方向键：移动光标。
  - 空格：切换墙和通路。
  - `Q`：返回主菜单。

坐标从 0 开始。起点和终点不能改成墙。编辑、重新生成或读取迷宫后，旧路径会被清除。

### 寻路与动画

- 选项 `4`：BFS 最短路径。
- 选项 `5`：Dijkstra 最短路径。
- 选项 `6`：A* 最短路径。
- 选项 `7`：比较三种算法的路径长度和运行时间。
- 选项 `11`：逐格播放当前路径；必须先运行一种寻路算法。

当前迷宫中每一步代价均为 1，因此三种算法得到的最短路径长度应一致。计时只包含求解过程，不包含迷宫打印时间；小迷宫可能显示 `0 us`。

### 保存与读取

- 选项 `8`：保存当前迷宫。
- 选项 `9`：读取迷宫文件。

文件名支持空格，相对路径按项目根目录解析。文件首行记录高度和宽度，后续逐行保存迷宫字符及空格。读取文件必须恰好包含一个 `S` 和一个 `E`；读取失败时保留当前迷宫。

## 项目结构

```text
include/core/       公共类型、工具和文件读写接口
include/model/      Maze 数据模型
include/generator/  DFS 迷宫生成器
include/solver/     BFS、Dijkstra、A* 求解器
src/                对应实现和主程序
tests/              自动测试与独立演示
docs/               接口、算法和测试文档
```

`tests/demo_b.cpp` 是独立示例，不能与 `src/main.cpp` 一起编译。

详细接口见 `docs/interface.md`，测试步骤见 `docs/testing.md`。

## 分支

- `main`：稳定主分支。
- `feature/member-a`：成员 A 开发分支。
- `feature/member-b`：成员 B 开发分支。

个人分支完成开发后，应先编译和测试，再合并到 `main`。
