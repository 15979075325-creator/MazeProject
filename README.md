# MazeProject



迷宫求解与可视化项目（C++17）。Windows 需要可从 PATH 调用的 g++。

双击 build.bat：编译并运行 DFS 生成与 BFS 求解演示。
双击 test.bat：运行求解器、生成器、Maze 和文件读写测试。
测试失败时脚本返回非零退出码。

`#` 为墙、空格为通路、`S` 为起点、`E` 为终点、`.` 为路径。
生成和寻路分别计时，不包含迷宫打印时间。
main.cpp 固定生成 21×21 迷宫；宽高必须是不小于 3 的奇数。
将 bfsFind 换为 dijkstraFind 可切换算法；当前每步代价为 1。

文件读写 API 已实现，使用 core/FileIO.h：

```cpp
saveMaze(grid, "maze.txt");
Grid loaded;
bool ok = loadMaze("maze.txt", loaded);
```

文件首行记录高度和宽度，其后完整保存字符与空格。读取失败不修改 loaded。
菜单：1 输入坐标编辑格子，2 BFS 求解，3 保存，4 读取，0 退出。
文件名支持空格；相对路径按项目根目录解析。
读取要求恰好一个 S 和一个 E，失败保留当前迷宫；旧路径标记会清除。
当前编辑方式是输入坐标，尚无方向键光标编辑和 A*。详细接口见 docs/interface.md。
tests/demo_b.cpp 是独立示例，不能与 src/main.cpp 一起编译。
