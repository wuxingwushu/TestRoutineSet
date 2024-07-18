#include <graphics.h>
#pragma comment (lib,"EasyXa.lib")
#include <iostream>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "genann.h"

const int SIZE_ = 4; // 方格大小
const int WIDTH = 100; // 方格宽度
const int MARGIN = 10; // 方格之间的间距
const int BOARD_X = 50; // 棋盘左上角 x 坐标
const int BOARD_Y = 50; // 棋盘左上角 y 坐标

int GetBoard(int* Data, int x, int y) {
    return Data[x * SIZE_ + y];
}

void SetBoard(int* Data, int x, int y, int z) {
    Data[x * SIZE_ + y] = z;
}

void initBoard(int* board_) {
    srand(time(NULL));
    int x1 = rand() % SIZE_;
    int y1 = rand() % SIZE_;
    int x2, y2;
    do {
        x2 = rand() % SIZE_;
        y2 = rand() % SIZE_;
    } while (x1 == x2 && y1 == y2);
    SetBoard(board_, x1, y1, 2);
    SetBoard(board_, x2, y2, 2);
}

void drawSquare(int x, int y, int num) {
    // 根据数字选择方块颜色
    COLORREF fillColor;
    switch (num) {
    case 2:
        fillColor = RGB(238, 228, 218); // 浅米色
        break;
    case 4:
        fillColor = RGB(237, 224, 200); // 浅灰色
        break;
    case 8:
        fillColor = RGB(242, 177, 121); // 橙色
        break;
    case 16:
        fillColor = RGB(245, 149, 99); // 深橙色
        break;
    case 32:
        fillColor = RGB(246, 124, 95); // 红橙色
        break;
    case 64:
        fillColor = RGB(246, 94, 59); // 红色
        break;
    case 128:
        fillColor = RGB(237, 207, 114); // 黄色
        break;
    case 256:
        fillColor = RGB(237, 204, 97); // 金色
        break;
    case 512:
        fillColor = RGB(186, 233, 113); // 浅绿色
        break;
    case 1024:
        fillColor = RGB(165, 212, 89); // 绿色
        break;
    case 2048:
        fillColor = RGB(123, 200, 79); // 深绿色
        break;
    default:
        fillColor = RGB(255, 255, 255); // 黑色（默认）
        break;
    }
    setfillcolor(fillColor);
    setlinecolor(WHITE);
    fillrectangle(BOARD_X + x * (WIDTH + MARGIN), BOARD_Y + y * (WIDTH + MARGIN),
        BOARD_X + (x + 1) * WIDTH + x * MARGIN,
        BOARD_Y + (y + 1) * WIDTH + y * MARGIN);
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    char str[10];
    sprintf(str, "%d", num);
    outtextxy(BOARD_X + x * (WIDTH + MARGIN) + WIDTH / 2 - 10,
        BOARD_Y + y * (WIDTH + MARGIN) + WIDTH / 2 - 10, str);
}

void drawBoard(int* board_) {
    for (int i = 0; i < SIZE_; ++i) {
        for (int j = 0; j < SIZE_; ++j) {
            drawSquare(j, i, GetBoard(board_, i, j));
        }
    }
}

struct Fraction
{
    int Branch = 0;
    bool moved = false;
};

Fraction moveUp(int* board_) {
    Fraction fraction;
    for (int j = 0; j < SIZE_; ++j) {
        for (int i = 1; i < SIZE_; ++i) {
            if (GetBoard(board_, i, j) != 0) {
                int k = i - 1;
                // 找到当前方块上方的空位置
                while (k >= 0 && GetBoard(board_, k, j) == 0) {
                    --k;
                }
                if (k >= 0 && GetBoard(board_, k, j) == GetBoard(board_, i, j)) {
                    // 如果上方有相同数字的方块，则合并
                    SetBoard(board_, k, j, GetBoard(board_, k, j) * 2);
                    SetBoard(board_, i, j, 0);
                    fraction.moved = true;
                    ++fraction.Branch;
                }
                else {
                    // 否则移动方块到上方空位置
                    SetBoard(board_, k + 1, j, GetBoard(board_, i, j));
                    if (k + 1 != i) {
                        SetBoard(board_, i, j, 0);
                        fraction.moved = true;
                    }
                }
            }
        }
    }
    return fraction;
}


Fraction moveDown(int* board_) {
    Fraction fraction;
    for (int j = 0; j < SIZE_; ++j) {
        for (int i = SIZE_ - 2; i >= 0; --i) {
            if (GetBoard(board_, i, j) != 0) {
                int k = i + 1;
                // 找到当前方块下方的空位置
                while (k < SIZE_ && GetBoard(board_, k, j) == 0) {
                    ++k;
                }
                if (k < SIZE_ && GetBoard(board_, k, j) == GetBoard(board_, i, j)) {
                    // 如果下方有相同数字的方块，则合并
                    SetBoard(board_, k, j, GetBoard(board_, k, j) * 2);
                    SetBoard(board_, i, j, 0);
                    fraction.moved = true;
                    ++fraction.Branch;
                }
                else {
                    // 否则移动方块到下方空位置
                    SetBoard(board_, k - 1, j, GetBoard(board_, i, j));
                    if (k - 1 != i) {
                        SetBoard(board_, i, j, 0);
                        fraction.moved = true;
                    }
                }
            }
        }
    }
    return fraction;
}

Fraction moveLeft(int* board_) {
    Fraction fraction;
    for (int i = 0; i < SIZE_; ++i) {
        for (int j = 1; j < SIZE_; ++j) {
            if (GetBoard(board_, i, j) != 0) {
                int k = j - 1;
                // 找到当前方块左侧的空位置
                while (k >= 0 && GetBoard(board_, i, k) == 0) {
                    --k;
                }
                if (k >= 0 && GetBoard(board_, i, k) == GetBoard(board_, i, j)) {
                    // 如果左侧有相同数字的方块，则合并
                    SetBoard(board_, i, k, GetBoard(board_, i, k) * 2);
                    SetBoard(board_, i, j, 0);
                    fraction.moved = true;
                    ++fraction.Branch;
                }
                else {
                    // 否则移动方块到左侧空位置
                    SetBoard(board_, i, k + 1, GetBoard(board_, i, j));
                    if (k + 1 != j) {
                        SetBoard(board_, i, j, 0);
                        fraction.moved = true;
                    }
                }
            }
        }
    }
    return fraction;
}

Fraction moveRight(int* board_) {
    Fraction fraction;
    for (int i = 0; i < SIZE_; ++i) {
        for (int j = SIZE_ - 2; j >= 0; --j) {
            if (GetBoard(board_, i, j) != 0) {
                int k = j + 1;
                // 找到当前方块右侧的空位置
                while (k < SIZE_ && GetBoard(board_, i, k) == 0) {
                    ++k;
                }
                if (k < SIZE_ && GetBoard(board_, i, k) == GetBoard(board_, i, j)) {
                    // 如果右侧有相同数字的方块，则合并
                    SetBoard(board_, i, k, GetBoard(board_, i, k) * 2);
                    SetBoard(board_, i, j, 0);
                    fraction.moved = true;
                    ++fraction.Branch;
                }
                else {
                    // 否则移动方块到右侧空位置
                    SetBoard(board_, i, k - 1, GetBoard(board_, i, j));
                    if (k - 1 != j) {
                        SetBoard(board_, i, j, 0);
                        fraction.moved = true;
                    }
                }
            }
        }
    }
    return fraction;
}

int isGameOver(int* board_) {
    // 检查棋盘上是否有空位置
    for (int i = 0; i < SIZE_; ++i) {
        for (int j = 0; j < SIZE_; ++j) {
            if (GetBoard(board_, i, j) == 0) {
                return false;  // 还有空位置
            }
        }
    }

    // 检查是否有可以合并的相邻方块
    for (int i = 0; i < SIZE_; ++i) {
        for (int j = 0; j < SIZE_ - 1; ++j) {
            if (GetBoard(board_, i, j) == GetBoard(board_, i, j + 1)) {
                return false;  // 有可以合并的水平方块
            }
        }
    }

    for (int j = 0; j < SIZE_; ++j) {
        for (int i = 0; i < SIZE_ - 1; ++i) {
            if (GetBoard(board_, i, j) == GetBoard(board_, i + 1, j)) {
                return false;  // 有可以合并的垂直方块
            }
        }
    }
    int sizeJG = 0;
    for (size_t i = 0; i < SIZE_* SIZE_; i++)
    {
        if (sizeJG < board_[i]) {
            sizeJG = board_[i];
        }
    }
    printf("结果：%d\n", sizeJG);

    // 如果没有空位置，也没有可以合并的方块，游戏结束
    return true;
}



void generateRandomSquare(int* board_) {
    // 初始化随机数生成器
    std::srand(std::time(0));

    // 找到所有空位置
    std::vector<std::pair<int, int>> emptyPositions;
    for (int i = 0; i < SIZE_; ++i) {
        for (int j = 0; j < SIZE_; ++j) {
            if (GetBoard(board_, i, j) == 0) {
                emptyPositions.push_back({ i, j });
            }
        }
    }

    if (emptyPositions.empty()) {
        return;  // 没有空位置
    }

    // 从空位置中随机选择一个
    int randomIndex = std::rand() % emptyPositions.size();
    int row = emptyPositions[randomIndex].first;
    int col = emptyPositions[randomIndex].second;

    // 生成 2 或 4
    int newValue = (std::rand() % 2 == 0) ? 2 : 4;

    // 在随机位置放置新方块
    SetBoard(board_, row, col, newValue);
}



void gameLoop() {
    int board[SIZE_ * SIZE_] = { 0 }; // 游戏棋盘
    initBoard(board);
    drawBoard(board);
    while (true) {
        bool moved = false;
        // 处理用户输入
        if (kbhit()) {
            char ch = getch();
            // 根据用户输入移动方块
            switch (ch) {
            case 'w':
                moved = moveUp(board).moved;
                break;
            case 's':
                moved = moveDown(board).moved;
                break;
            case 'a':
                moved = moveLeft(board).moved;
                break;
            case 'd':
                moved = moveRight(board).moved;
                break;
            default:
                break;
            }
            // 检查游戏是否结束
            if (isGameOver(board)) {
                // 游戏结束，提示并退出循环
                outtextxy(200, 250, "Game Over!");
                break;
            }
            // 如果有方块移动或合并，则生成新的数字方块
            if (moved) {
                generateRandomSquare(board);
            }
            // 重绘游戏界面
            cleardevice();
            drawBoard(board);
        }
    }
}

struct Forecast
{
    int Profit = 0;
    unsigned char Id = 0;
};

Forecast AlgorithmForecast(int* board_, int Frequency) {
    int mBoard_[4][SIZE_ * SIZE_];
    Fraction mFraction[4];
    for (size_t i = 0; i < 4; i++){
        memcpy(mBoard_[i], board_, SIZE_ * SIZE_ * sizeof(int));
        if (i == 0)mFraction[0] = moveUp(mBoard_[0]);
        if (i == 1)mFraction[1] = moveDown(mBoard_[1]);
        if (i == 2)mFraction[2] = moveLeft(mBoard_[2]);
        if (i == 3)mFraction[3] = moveRight(mBoard_[3]);
        if (mFraction[i].moved) { if (Frequency > 0) { generateRandomSquare(mBoard_[i]); mFraction[i].Branch += AlgorithmForecast(mBoard_[i], Frequency - 1).Profit; } }
        else mFraction[i].Branch -= 100;
    }
    Forecast mForecast = { mFraction[0].Branch, 0 };
    for (size_t i = 0; i < 3; i++) {
        if (mFraction[i].Branch < mFraction[i + 1].Branch) {
            mForecast.Profit = mFraction[i + 1].Branch;
            mForecast.Id = i + 1;
        }
    }
    return mForecast;
}

genann* ann = nullptr;

void GameAI() {
    int board_BF[SIZE_*SIZE_] = { 0 }; // 游戏棋盘
    initBoard(board_BF);
    bool moved = 0;
    while (!isGameOver(board_BF))
    {
        Forecast mForecast = AlgorithmForecast(board_BF, 6);
        double Label[SIZE_];
        double Train[SIZE_ * SIZE_];
        for (size_t i = 0; i < SIZE_; i++)
        {
            Label[i] = i == mForecast.Id ? 1 : 0;
        }
        for (size_t i = 0; i < (SIZE_ * SIZE_); i++)
        {
            Train[i] = double(board_BF[i]) / 2048;
        }
        genann_train(ann, Train, Label, 0.01);
        switch (mForecast.Id) {
        case 0:
            moved = moveUp(board_BF).moved;
            break;
        case 1:
            moved = moveDown(board_BF).moved;
            break;
        case 2:
            moved = moveLeft(board_BF).moved;
            break;
        case 3:
            moved = moveRight(board_BF).moved;
            break;
        default:
            break;
        }

        // 如果有方块移动或合并，则生成新的数字方块
        if (moved) {
            generateRandomSquare(board_BF);
        }
    }
    return;
}

void TestGameAI() {
    int board_BF[SIZE_ * SIZE_] = { 0 }; // 游戏棋盘
    initBoard(board_BF);
    bool moved = 0;
    int ZCS = 0, DCS = 0;
    int LS;
    while (!isGameOver(board_BF))
    {
        Forecast mForecast = AlgorithmForecast(board_BF, 6);
        const double* Label;
        double Train[SIZE_ * SIZE_];
        for (size_t i = 0; i < (SIZE_ * SIZE_); i++)
        {
            Train[i] = double(board_BF[i]) / 2048;
        }
        Label = genann_run(ann, Train);
        LS = 0;
        for (size_t i = 0; i < 3; i++) {
            if (Label[i] < Label[i+1]) {
                LS = i+1;
            }
        }
        ++ZCS;
        if (LS == mForecast.Id) ++DCS;
        switch (mForecast.Id) {
        case 0:
            moved = moveUp(board_BF).moved;
            break;
        case 1:
            moved = moveDown(board_BF).moved;
            break;
        case 2:
            moved = moveLeft(board_BF).moved;
            break;
        case 3:
            moved = moveRight(board_BF).moved;
            break;
        default:
            break;
        }

        // 如果有方块移动或合并，则生成新的数字方块
        if (moved) {
            generateRandomSquare(board_BF);
        }
    }
    printf("%f\n", float(DCS) / ZCS);
    return;
}

int main() {
    if (0) {
        srand(time(0));
        ann = genann_init(16, 3, 784, 4);
    }
    else {
        FILE* fpa = fopen("./mods.ann", "r");
        ann = genann_read(fpa);
        fclose(fpa);
    }
    for (int i = 0; i < 1000; ++i)
    {
        printf("%d\n", i);
        GameAI();
        if (i % 10 == 0) {
            TestGameAI();
            FILE* fpw = fopen("mods.ann", "w");
            genann_write(ann, fpw);
            fclose(fpw);
        }
    } 
    FILE* fpw = fopen("mods.ann", "w");
    genann_write(ann, fpw);
    fclose(fpw);
    /*initgraph(600, 600);
    gameLoop();
    closegraph();*/
    return 0;
}

