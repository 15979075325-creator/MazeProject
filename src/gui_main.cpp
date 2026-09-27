#define UNICODE
#define _UNICODE

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>

#include "core/FileIO.h"
#include "core/Utils.h"
#include "generator/DfsGenerator.h"
#include "solver/AStar.h"
#include "solver/Solver.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int kNewMazeButton = 1001;
constexpr int kBfsButton = 1002;
constexpr int kDijkstraButton = 1003;
constexpr int kAStarButton = 1004;
constexpr int kClearPathButton = 1005;
constexpr int kToggleWallButton = 1006;
constexpr int kSetStartButton = 1007;
constexpr int kSetEndButton = 1008;
constexpr int kSaveButton = 1009;
constexpr int kLoadButton = 1010;
constexpr int kCompareButton = 1011;
constexpr int kAppIconResource = 101;
constexpr int kRowsInput = 1101;
constexpr int kColsInput = 1102;
constexpr int kSeedInput = 1103;
constexpr UINT_PTR kPathAnimationTimer = 2001;
constexpr int kToolbarHeight = 120;

enum class EditMode {
    ToggleWall,
    SetStart,
    SetEnd
};

struct MazeLayout {
    int cellSize;
    int left;
    int top;
    int rows;
    int cols;
};

Grid mazeGrid;
std::vector<Pos> currentPath;
std::vector<std::vector<unsigned char>> visiblePathMask;
std::size_t visiblePathCells = 0;
Pos startPos{1, 1};
Pos endPos{19, 19};
std::wstring statusText = L"Ready";
EditMode editMode = EditMode::ToggleWall;

void resetVisiblePath() {
    visiblePathCells = 0;
    visiblePathMask.assign(
        mazeGrid.size(),
        std::vector<unsigned char>(mazeGrid.empty() ? 0 : mazeGrid.front().size(), 0));
}

void beginPathAnimation(HWND window) {
    resetVisiblePath();
    if (currentPath.empty()) {
        return;
    }

    const Pos first = currentPath.front();
    visiblePathMask[first.first][first.second] = 1;
    visiblePathCells = 1;
    SetTimer(window, kPathAnimationTimer, 25, nullptr);
}

void generateMaze(unsigned int rows, unsigned int cols, unsigned int requestedSeed) {
    unsigned int actualSeed = requestedSeed;
    if (actualSeed == 0) {
        actualSeed = static_cast<unsigned int>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
        if (actualSeed == 0) {
            actualSeed = 1;
        }
    }
    setSeed(actualSeed);

    DfsGenerator generator(cols, rows);
    generator.generate();
    mazeGrid = generator.grid();
    startPos = generator.start();
    endPos = generator.goal();
    mazeGrid[startPos.first][startPos.second] = 'S';
    mazeGrid[endPos.first][endPos.second] = 'E';
    currentPath.clear();
    resetVisiblePath();
    std::wstringstream message;
    message << L"New " << rows << L" x " << cols << L" maze, seed " << actualSeed;
    statusText = message.str();
}

void generateMazeFromInputs(HWND window) {
    BOOL rowsValid = FALSE;
    BOOL colsValid = FALSE;
    BOOL seedValid = FALSE;
    const UINT rows = GetDlgItemInt(window, kRowsInput, &rowsValid, FALSE);
    const UINT cols = GetDlgItemInt(window, kColsInput, &colsValid, FALSE);
    const UINT seed = GetDlgItemInt(window, kSeedInput, &seedValid, FALSE);

    if (!rowsValid || !colsValid || !seedValid) {
        statusText = L"Rows, cols and seed must be whole numbers";
        return;
    }
    if (rows < 3 || rows > 51 || cols < 3 || cols > 51 ||
        rows % 2 == 0 || cols % 2 == 0) {
        statusText = L"Rows and cols must be odd numbers from 3 to 51";
        return;
    }

    generateMaze(rows, cols, seed);
}

std::string ensureSaveDirectory() {
    char executablePath[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameA(nullptr, executablePath, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return {};
    }

    std::string directory(executablePath, length);
    const std::size_t separator = directory.find_last_of("\\/");
    if (separator == std::string::npos) {
        return {};
    }

    directory.resize(separator);
    directory += "\\saved_mazes";
    if (!CreateDirectoryA(directory.c_str(), nullptr) &&
        GetLastError() != ERROR_ALREADY_EXISTS) {
        return {};
    }
    return directory;
}

std::string chooseSaveFile(HWND window) {
    const std::string saveDirectory = ensureSaveDirectory();
    char filename[MAX_PATH] = "maze.txt";
    OPENFILENAMEA dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = window;
    dialog.lpstrFilter =
        "Maze files (*.maze;*.txt)\0*.maze;*.txt\0All files (*.*)\0*.*\0";
    dialog.lpstrFile = filename;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrInitialDir = saveDirectory.empty() ? nullptr : saveDirectory.c_str();
    dialog.lpstrDefExt = "txt";
    dialog.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    return GetSaveFileNameA(&dialog) ? std::string(filename) : std::string();
}

std::string chooseOpenFile(HWND window) {
    const std::string saveDirectory = ensureSaveDirectory();
    char filename[MAX_PATH] = {};
    OPENFILENAMEA dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = window;
    dialog.lpstrFilter =
        "Maze files (*.maze;*.txt)\0*.maze;*.txt\0All files (*.*)\0*.*\0";
    dialog.lpstrFile = filename;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrInitialDir = saveDirectory.empty() ? nullptr : saveDirectory.c_str();
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    return GetOpenFileNameA(&dialog) ? std::string(filename) : std::string();
}

bool locateEndpoints(Grid& grid, Pos& start, Pos& end) {
    int startCount = 0;
    int endCount = 0;
    for (int row = 0; row < static_cast<int>(grid.size()); ++row) {
        for (int col = 0; col < static_cast<int>(grid[row].size()); ++col) {
            char& cell = grid[row][col];
            if (cell == 'S') {
                start = {row, col};
                ++startCount;
            } else if (cell == 'E') {
                end = {row, col};
                ++endCount;
            } else if (cell == '.') {
                cell = ' ';
            }
        }
    }
    return startCount == 1 && endCount == 1;
}

void saveCurrentMaze(HWND window) {
    const std::string filename = chooseSaveFile(window);
    if (filename.empty()) {
        statusText = L"Save cancelled";
        return;
    }
    statusText = saveMaze(mazeGrid, filename) ? L"Maze saved" : L"Could not save maze";
}

void loadMazeFromFile(HWND window) {
    const std::string filename = chooseOpenFile(window);
    if (filename.empty()) {
        statusText = L"Load cancelled";
        return;
    }

    Grid loaded;
    Pos loadedStart{};
    Pos loadedEnd{};
    if (!loadMaze(filename, loaded) ||
        !locateEndpoints(loaded, loadedStart, loadedEnd)) {
        statusText = L"Invalid maze file: exactly one S and one E are required";
        return;
    }

    KillTimer(window, kPathAnimationTimer);
    mazeGrid = std::move(loaded);
    startPos = loadedStart;
    endPos = loadedEnd;
    currentPath.clear();
    resetVisiblePath();
    SetDlgItemInt(window, kRowsInput, static_cast<UINT>(mazeGrid.size()), FALSE);
    SetDlgItemInt(window, kColsInput, static_cast<UINT>(mazeGrid.front().size()), FALSE);
    statusText = L"Maze loaded";
}

void solveMaze(HWND window, int algorithm) {
    KillTimer(window, kPathAnimationTimer);
    if (algorithm == kBfsButton) {
        currentPath = bfsFind(mazeGrid, startPos, endPos);
        statusText = L"BFS";
    } else if (algorithm == kDijkstraButton) {
        currentPath = dijkstraFind(mazeGrid, startPos, endPos);
        statusText = L"Dijkstra";
    } else {
        currentPath = aStarFind(mazeGrid, startPos, endPos);
        statusText = L"A*";
    }

    if (currentPath.empty()) {
        resetVisiblePath();
        statusText += L": no path";
    } else {
        std::wstringstream message;
        message << statusText << L": path length " << currentPath.size();
        statusText = message.str();
        beginPathAnimation(window);
    }
}

template <typename Finder>
std::pair<std::vector<Pos>, long long> measurePath(Finder finder) {
    const auto started = std::chrono::steady_clock::now();
    std::vector<Pos> path = finder(mazeGrid, startPos, endPos);
    const auto finished = std::chrono::steady_clock::now();
    const long long microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(finished - started).count();
    return {std::move(path), microseconds};
}

void compareAlgorithms(HWND window) {
    KillTimer(window, kPathAnimationTimer);
    const auto bfsResult = measurePath(bfsFind);
    const auto dijkstraResult = measurePath(dijkstraFind);
    const auto aStarResult = measurePath(aStarFind);

    const bool sameLength =
        bfsResult.first.size() == dijkstraResult.first.size() &&
        bfsResult.first.size() == aStarResult.first.size();

    std::wstringstream report;
    report << L"BFS: " << bfsResult.first.size() << L" cells, "
           << bfsResult.second << L" us\n"
           << L"Dijkstra: " << dijkstraResult.first.size() << L" cells, "
           << dijkstraResult.second << L" us\n"
           << L"A*: " << aStarResult.first.size() << L" cells, "
           << aStarResult.second << L" us\n\n"
           << (sameLength ? L"All path lengths match."
                          : L"Warning: path lengths do not match.");

    MessageBoxW(window, report.str().c_str(), L"Algorithm comparison",
                MB_OK | (sameLength ? MB_ICONINFORMATION : MB_ICONWARNING));

    currentPath = aStarResult.first;
    statusText = L"Comparison complete; displaying the A* path";
    beginPathAnimation(window);
}

bool isPathCell(int row, int col) {
    return row >= 0 && row < static_cast<int>(visiblePathMask.size()) &&
           col >= 0 && !visiblePathMask.empty() &&
           col < static_cast<int>(visiblePathMask[row].size()) &&
           visiblePathMask[row][col] != 0;
}

void fillRect(HDC dc, const RECT& rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

MazeLayout calculateLayout(HWND window) {
    RECT client{};
    GetClientRect(window, &client);

    const int rows = static_cast<int>(mazeGrid.size());
    const int cols = rows == 0 ? 0 : static_cast<int>(mazeGrid.front().size());
    if (rows == 0 || cols == 0) {
        return {0, 0, 0, rows, cols};
    }

    const int availableWidth = std::max(1, static_cast<int>(client.right) - 32);
    const int availableHeight =
        std::max(1, static_cast<int>(client.bottom) - kToolbarHeight - 48);
    const int cellSize = std::max(4, std::min(availableWidth / cols, availableHeight / rows));
    const int mazeWidth = cellSize * cols;
    return {
        cellSize,
        (static_cast<int>(client.right) - mazeWidth) / 2,
        kToolbarHeight + 8,
        rows,
        cols
    };
}

bool cellFromPoint(HWND window, int x, int y, Pos& position) {
    const MazeLayout layout = calculateLayout(window);
    if (layout.cellSize == 0 || x < layout.left || y < layout.top) {
        return false;
    }

    const int col = (x - layout.left) / layout.cellSize;
    const int row = (y - layout.top) / layout.cellSize;
    if (row < 0 || row >= layout.rows || col < 0 || col >= layout.cols) {
        return false;
    }

    position = {row, col};
    return true;
}

void editCell(HWND window, int x, int y) {
    Pos position{};
    if (!cellFromPoint(window, x, y, position)) {
        return;
    }

    const int row = position.first;
    const int col = position.second;

    if (editMode == EditMode::ToggleWall) {
        if (position == startPos || position == endPos) {
            statusText = L"Start and end cannot be walls";
            return;
        }
        mazeGrid[row][col] = mazeGrid[row][col] == '#' ? ' ' : '#';
        statusText = mazeGrid[row][col] == '#' ? L"Wall added" : L"Road opened";
    } else if (editMode == EditMode::SetStart) {
        if (mazeGrid[row][col] == '#' || position == endPos) {
            statusText = L"Start must be placed on an open cell";
            return;
        }
        mazeGrid[startPos.first][startPos.second] = ' ';
        startPos = position;
        mazeGrid[row][col] = 'S';
        statusText = L"Start moved";
    } else {
        if (mazeGrid[row][col] == '#' || position == startPos) {
            statusText = L"End must be placed on an open cell";
            return;
        }
        mazeGrid[endPos.first][endPos.second] = ' ';
        endPos = position;
        mazeGrid[row][col] = 'E';
        statusText = L"End moved";
    }

    KillTimer(window, kPathAnimationTimer);
    currentPath.clear();
    resetVisiblePath();
    InvalidateRect(window, nullptr, TRUE);
}

void drawMaze(HWND window, HDC dc) {
    RECT client{};
    GetClientRect(window, &client);
    fillRect(dc, client, RGB(241, 245, 249));

    if (mazeGrid.empty()) {
        return;
    }

    const MazeLayout layout = calculateLayout(window);
    const int rows = layout.rows;
    const int cols = layout.cols;
    const int cellSize = layout.cellSize;
    const int left = layout.left;
    const int top = layout.top;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            RECT cell{
                left + col * cellSize,
                top + row * cellSize,
                left + (col + 1) * cellSize,
                top + (row + 1) * cellSize
            };

            COLORREF color = RGB(255, 255, 255);
            const char value = mazeGrid[row][col];
            if (value == '#') {
                color = RGB(30, 41, 59);
            } else if (Pos{row, col} == startPos) {
                color = RGB(34, 197, 94);
            } else if (Pos{row, col} == endPos) {
                color = RGB(239, 68, 68);
            } else if (isPathCell(row, col)) {
                color = RGB(250, 204, 21);
            }

            fillRect(dc, cell, color);

            HPEN pen = CreatePen(PS_SOLID, 1, RGB(203, 213, 225));
            HGDIOBJ oldPen = SelectObject(dc, pen);
            HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
            Rectangle(dc, cell.left, cell.top, cell.right, cell.bottom);
            SelectObject(dc, oldBrush);
            SelectObject(dc, oldPen);
            DeleteObject(pen);
        }
    }

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(15, 23, 42));
    RECT statusRect{16, client.bottom - 32, client.right - 16, client.bottom - 8};
    DrawTextW(dc, statusText.c_str(), -1, &statusRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void createButton(HWND parent, const wchar_t* label, int id, int x, int width,
                  int y = 18) {
    CreateWindowW(
        L"BUTTON", label,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x, y, width, 36,
        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr), nullptr);
}

void createEditButton(HWND parent, const wchar_t* label, int id, int x, int width,
                      bool beginsGroup = false) {
    DWORD style = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON;
    if (beginsGroup) {
        style |= WS_GROUP;
    }
    CreateWindowW(
        L"BUTTON", label, style,
        x, 66, width, 32,
        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr), nullptr);
}

void createLabel(HWND parent, const wchar_t* text, int x, int width) {
    CreateWindowW(
        L"STATIC", text, WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, 70, width, 24,
        parent, nullptr, GetModuleHandleW(nullptr), nullptr);
}

void createNumberInput(HWND parent, const wchar_t* initialValue, int id, int x, int width) {
    CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", initialValue,
        WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_CENTER | WS_TABSTOP,
        x, 66, width, 30,
        parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(nullptr), nullptr);
}

LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        createButton(window, L"New maze", kNewMazeButton, 16, 112);
        createButton(window, L"BFS", kBfsButton, 140, 82);
        createButton(window, L"Dijkstra", kDijkstraButton, 234, 104);
        createButton(window, L"A*", kAStarButton, 350, 82);
        createButton(window, L"Clear path", kClearPathButton, 444, 112);
        createButton(window, L"Save", kSaveButton, 568, 82);
        createButton(window, L"Load", kLoadButton, 662, 82);
        createButton(window, L"Compare", kCompareButton, 16, 112, 64);
        createEditButton(window, L"Wall / road", kToggleWallButton, 140, 116, true);
        createEditButton(window, L"Set start", kSetStartButton, 268, 104);
        createEditButton(window, L"Set end", kSetEndButton, 384, 104);
        createLabel(window, L"Rows", 500, 42);
        createNumberInput(window, L"21", kRowsInput, 542, 44);
        createLabel(window, L"Cols", 592, 40);
        createNumberInput(window, L"21", kColsInput, 632, 44);
        createLabel(window, L"Seed", 684, 40);
        createNumberInput(window, L"0", kSeedInput, 724, 72);
        CheckRadioButton(window, kToggleWallButton, kSetEndButton, kToggleWallButton);
        generateMaze(21, 21, 0);
        return 0;

    case WM_COMMAND: {
        const int command = LOWORD(wParam);
        if (command == kNewMazeButton) {
            KillTimer(window, kPathAnimationTimer);
            generateMazeFromInputs(window);
        } else if (command == kBfsButton || command == kDijkstraButton ||
                   command == kAStarButton) {
            solveMaze(window, command);
        } else if (command == kClearPathButton) {
            KillTimer(window, kPathAnimationTimer);
            currentPath.clear();
            resetVisiblePath();
            statusText = L"Path cleared";
        } else if (command == kSaveButton) {
            saveCurrentMaze(window);
        } else if (command == kLoadButton) {
            loadMazeFromFile(window);
        } else if (command == kCompareButton) {
            compareAlgorithms(window);
        } else if (command == kToggleWallButton || command == kSetStartButton ||
                   command == kSetEndButton) {
            CheckRadioButton(window, kToggleWallButton, kSetEndButton, command);
            if (command == kToggleWallButton) {
                editMode = EditMode::ToggleWall;
                statusText = L"Edit mode: wall / road";
            } else if (command == kSetStartButton) {
                editMode = EditMode::SetStart;
                statusText = L"Edit mode: set start";
            } else {
                editMode = EditMode::SetEnd;
                statusText = L"Edit mode: set end";
            }
        }
        InvalidateRect(window, nullptr, TRUE);
        return 0;
    }

    case WM_LBUTTONDOWN:
        editCell(window, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_TIMER:
        if (wParam == kPathAnimationTimer) {
            if (visiblePathCells < currentPath.size()) {
                const Pos next = currentPath[visiblePathCells];
                visiblePathMask[next.first][next.second] = 1;
                ++visiblePathCells;
                InvalidateRect(window, nullptr, FALSE);
            } else {
                KillTimer(window, kPathAnimationTimer);
            }
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(window, &paint);

        RECT client{};
        GetClientRect(window, &client);
        const int width = static_cast<int>(client.right - client.left);
        const int height = static_cast<int>(client.bottom - client.top);

        if (width > 0 && height > 0) {
            HDC memoryDc = CreateCompatibleDC(dc);
            HBITMAP backBuffer = CreateCompatibleBitmap(dc, width, height);
            HGDIOBJ oldBitmap = SelectObject(memoryDc, backBuffer);

            drawMaze(window, memoryDc);
            BitBlt(dc, 0, 0, width, height, memoryDc, 0, 0, SRCCOPY);

            SelectObject(memoryDc, oldBitmap);
            DeleteObject(backBuffer);
            DeleteDC(memoryDc);
        }
        EndPaint(window, &paint);
        return 0;
    }

    case WM_ERASEBKGND:
        // WM_PAINT draws the complete frame through a back buffer.
        return 1;

    case WM_SIZE:
        InvalidateRect(window, nullptr, TRUE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(window, message, wParam, lParam);
    }
}

} // namespace

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCommand) {
    const wchar_t className[] = L"MazeProjectWindow";

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.hIcon = static_cast<HICON>(LoadImageW(
        instance, MAKEINTRESOURCEW(kAppIconResource), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
    windowClass.hIconSm = static_cast<HICON>(LoadImageW(
        instance, MAKEINTRESOURCEW(kAppIconResource), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));

    if (!RegisterClassExW(&windowClass)) {
        MessageBoxW(nullptr, L"Could not register the window class.",
                    L"MazeProject", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND window = CreateWindowExW(
        0, className, L"MazeProject - Graphical Version",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 820, 760,
        nullptr, nullptr, instance, nullptr);

    if (!window) {
        MessageBoxW(nullptr, L"Could not create the window.",
                    L"MazeProject", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
