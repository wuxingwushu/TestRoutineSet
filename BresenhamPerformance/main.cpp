#include <iostream>
#include <math.h>
#include "Timer.h"
#include <vector>

#define stitX 0
#define stitY 0
#define endX 499
#define endY 200

#define WQHG 500L
#define TESTWQHG 500L
#define ForNum 100000 / TESTWQHG
#define DebugSW 0

// 空間換速度
// 減少循環次數
// 減少循環一次的計算量
std::vector<std::vector<int>> WQData{};
std::vector<std::vector<bool>> WQBool{};
int GetHG(int x, int y)
{
    int min;
    if (x > y)
    {
        min = y;
    }
    else
    {
        min = x;
    }
    if (min > (WQHG - x))
    {
        min = (WQHG - x);
    }
    if (min > (WQHG - y))
    {
        min = (WQHG - y);
    }
    return min + 1;
}

void InitWQData()
{
    for (int x = 0; x < WQHG + 2; ++x)
    {
        WQData.push_back(std::vector<int>(WQHG + 2));
        WQBool.push_back(std::vector<bool>(WQHG + 2));
        for (int y = 0; y < WQHG + 2; ++y)
        {
            WQData[x][y] = GetHG(x, y);
        }
    }
}

void bresenham_YB(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    while (true)
    {
        if (x0 == x1 && y0 == y1)
            break;
        WQBool[x0][y0] = ~WQBool[x0][y0];
        e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void bresenham(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    char sx = (x0 < x1) ? 1 : -1;
    char sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    unsigned int Fi;

    while (true)
    {
        if (sx > 0)
        {
            if (x0 >= x1)
            {
                if (sy > 0)
                {
                    if (y0 >= y1)
                    {
                        break;
                    }
                }
                else
                {
                    if (y0 <= y1)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            if (x0 <= x1)
            {
                if (sy > 0)
                {
                    if (y0 >= y1)
                    {
                        break;
                    }
                }
                else
                {
                    if (y0 <= y1)
                    {
                        break;
                    }
                }
            }
        }
#if DebugSW
        std::cout << x0 << " , " << y0 << std::endl;
#endif
        WQBool[x0][y0] = ~WQBool[x0][y0];
        Fi = WQData[x0][y0];
        do
        {
            e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        } while (--Fi);
    }
}

void bresenham_line(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    char sx = (x0 < x1) ? 1 : -1;
    char sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    unsigned int Fi;

    while (true)
    {
        if (sx > 0)
        {
            if (x0 >= x1)
            {
                if (sy > 0)
                {
                    if (y0 >= y1)
                    {
                        break;
                    }
                }
                else
                {
                    if (y0 <= y1)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            if (x0 <= x1)
            {
                if (sy > 0)
                {
                    if (y0 >= y1)
                    {
                        break;
                    }
                }
                else
                {
                    if (y0 <= y1)
                    {
                        break;
                    }
                }
            }
        }
#if DebugSW
        std::cout << x0 << " , " << y0 << std::endl;
#endif
        WQBool[x0][y0] = ~WQBool[x0][y0];
        Fi = WQData[x0][y0];
        if (dy > dx)
        {
            y0 += (sy * Fi);
            do
            {
                if ((err * 2) > -dy)
                {
                    err -= dy;
                    x0 += sx;
                }
                err += dx;
            } while (--Fi);
        }
        else if (dy < dx)
        {
            x0 += (sx * Fi);
            do
            {
                if ((err * 2) < dx)
                {
                    err += dx;
                    y0 += sy;
                }
                err -= dy;
            } while (--Fi);
        }
        else
        {
            y0 += (Fi * sy);
            x0 += (Fi * sx);
        }
    }
}

void bresenham_line2(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    char sx = (x0 < x1) ? 1 : -1;
    char sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    unsigned int Fi;
    if (sx > 0)
    {
        if (sy > 0)
        {
            while (true)
            {
                if (x0 >= x1 && y0 >= y1)
                {
                    break;
                }
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                do
                {
                    e2 = 2 * err;
                    if (e2 > -dy)
                    {
                        err -= dy;
                        ++x0;
                    }
                    if (e2 < dx)
                    {
                        err += dx;
                        ++y0;
                    }
                } while (--Fi);
            }
        }
        else
        {
            while (true)
            {
                if (x0 >= x1 && y0 <= y1)
                {
                    break;
                } // 线条绘制完毕
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                do
                {
                    e2 = 2 * err;
                    if (e2 > -dy)
                    {
                        err -= dy;
                        ++x0;
                    }
                    if (e2 < dx)
                    {
                        err += dx;
                        --y0;
                    }
                } while (--Fi);
            }
        }
    }
    else
    {
        if (sy > 0)
        {
            while (true)
            {
                if (x0 <= x1 && y0 >= y1)
                {
                    break;
                } // 线条绘制完毕
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                do
                {
                    e2 = 2 * err;
                    if (e2 > -dy)
                    {
                        err -= dy;
                        --x0;
                    }
                    if (e2 < dx)
                    {
                        err += dx;
                        ++y0;
                    }
                } while (--Fi);
            }
        }
        else
        {
            while (true)
            {
                if (x0 <= x1 && y0 <= y1)
                {
                    break;
                } // 线条绘制完毕
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                do
                {
                    e2 = 2 * err;
                    if (e2 > -dy)
                    {
                        err -= dy;
                        --x0;
                    }
                    if (e2 < dx)
                    {
                        err += dx;
                        --y0;
                    }
                } while (--Fi);
            }
        }
    }
}

void bresenham_line3(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    char sx = (x0 < x1) ? 1 : -1;
    char sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    unsigned int Fi;

    if (sx > 0)
    {
        if (sy > 0)
        {
            while (true)
            {
                if (x0 >= x1 && y0 >= y1)
                {
                    break;
                }
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                if (dy > dx)
                {
                    y0 += Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            ++x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
                else if (dy < dx)
                {
                    x0 += Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            ++y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
                else
                {
                    y0 += Fi;
                    x0 += Fi;
                }
            }
        }
        else
        {
            while (true)
            {
                if (x0 >= x1 && y0 <= y1)
                {
                    break;
                }
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                if (dy > dx)
                {
                    y0 -= Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            ++x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
                else if (dy < dx)
                {
                    x0 += Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            --y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
                else
                {
                    y0 -= Fi;
                    x0 += Fi;
                }
            }
        }
    }
    else
    {
        if (sy > 0)
        {
            while (true)
            {
                if (x0 <= x1 && y0 >= y1)
                {
                    break;
                }
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                if (dy > dx)
                {
                    y0 += Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            --x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
                else if (dy < dx)
                {
                    x0 -= Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            ++y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
                else
                {
                    y0 += Fi;
                    x0 -= Fi;
                }
            }
        }
        else
        {
            while (true)
            {
                if (x0 <= x1 && y0 <= y1)
                {
                    break;
                }
#if DebugSW
                std::cout << x0 << " , " << y0 << std::endl;
#endif
                WQBool[x0][y0] = ~WQBool[x0][y0];
                Fi = WQData[x0][y0];
                if (dy > dx)
                {
                    y0 -= Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            --x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
                else if (dy < dx)
                {
                    x0 -= Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            --y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
                else
                {
                    y0 -= Fi;
                    x0 -= Fi;
                }
            }
        }
    }
}

void bresenham_line4(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    char sx = (x0 < x1) ? 1 : -1;
    char sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    unsigned int Fi;

    if (sx > 0)
    {
        if (sy > 0)
        {
            if (dy > dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 >= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    y0 += Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            ++x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
            }
            else if (dy < dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 >= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 += Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            ++y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
            }
            else
            {
                while (true)
                {
                    if (x0 >= x1 && y0 >= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 += Fi;
                    y0 += Fi;
                }
            }
        }
        else
        {
            if (dy > dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 <= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    y0 -= Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            ++x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
            }
            else if (dy < dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 <= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 += Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            --y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
            }
            else
            {
                while (true)
                {
                    if (x0 >= x1 && y0 <= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 += Fi;
                    y0 -= Fi;
                }
            }
        }
    }
    else
    {
        if (sy > 0)
        {
            if (dy > dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 >= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    y0 += Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            --x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
            }
            else if (dy < dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 >= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 -= Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            ++y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
            }
            else
            {
                while (true)
                {
                    if (x0 >= x1 && y0 >= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 -= Fi;
                    y0 += Fi;
                }
            }
        }
        else
        {
            if (dy > dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 <= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    y0 -= Fi;
                    do
                    {
                        if ((err * 2) > -dy)
                        {
                            err -= dy;
                            --x0;
                        }
                        err += dx;
                    } while (--Fi);
                }
            }
            else if (dy < dx)
            {
                while (true)
                {
                    if (x0 >= x1 && y0 <= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 -= Fi;
                    do
                    {
                        if ((err * 2) < dx)
                        {
                            err += dx;
                            --y0;
                        }
                        err -= dy;
                    } while (--Fi);
                }
            }
            else
            {
                while (true)
                {
                    if (x0 >= x1 && y0 <= y1)
                    {
                        break;
                    }
#if DebugSW
                    std::cout << x0 << " , " << y0 << std::endl;
#endif
                    WQBool[x0][y0] = ~WQBool[x0][y0];
                    Fi = WQData[x0][y0];
                    x0 -= Fi;
                    y0 -= Fi;
                }
            }
        }
    }
}

int main()
{
    InitWQData();
#if DebugSW
    bresenham(stitX, stitY, endX, endY);
    std::cout << "--------------------------" << std::endl;
    bresenham_line(stitX, stitY, endX, endY);
    std::cout << "--------------------------" << std::endl;
    bresenham_line2(stitX, stitY, endX, endY);
    std::cout << "--------------------------" << std::endl;
    bresenham_line3(stitX, stitY, endX, endY);
    std::cout << "--------------------------" << std::endl;
    bresenham_line4(stitX, stitY, endX, endY);
#else
    Timer mTimer(10);
    std::cout << "Y ing !" << std::endl;
    mTimer.MomentTiming("Y");
    for (int shu = 0; shu < ForNum; ++shu)
    {
        for (int i = 0; i < TESTWQHG; ++i)
        {
            bresenham_YB(0, 0, WQHG, i);
        }
    }
    mTimer.MomentEnd();
    std::cout << "1 ing !" << std::endl;
    mTimer.MomentTiming("1");
    for (int shu = 0; shu < ForNum; ++shu)
    {
        for (int i = 0; i < TESTWQHG; ++i)
        {
            bresenham(0, 0, WQHG, i);
        }
    }
    mTimer.MomentEnd();
    std::cout << "2 ing !" << std::endl;
    mTimer.MomentTiming("2");
    for (int shu = 0; shu < ForNum; ++shu)
    {
        for (int i = 0; i < TESTWQHG; ++i)
        {
            bresenham_line(0, 0, WQHG, i);
        }
    }
    mTimer.MomentEnd();
    std::cout << "3 ing !" << std::endl;
    mTimer.MomentTiming("3");
    for (int shu = 0; shu < ForNum; ++shu)
    {
        for (int i = 0; i < TESTWQHG; ++i)
        {
            bresenham_line2(0, 0, WQHG, i);
        }
    }
    mTimer.MomentEnd();
    std::cout << "4 ing !" << std::endl;
    mTimer.MomentTiming("4");
    for (int shu = 0; shu < ForNum; ++shu)
    {
        for (int i = 0; i < TESTWQHG; ++i)
        {
            bresenham_line3(0, 0, WQHG, i);
        }
    }
    mTimer.MomentEnd();
    std::cout << "5 ing !" << std::endl;
    mTimer.MomentTiming("5");
    for (int shu = 0; shu < ForNum; ++shu)
    {
        for (int i = 0; i < TESTWQHG; ++i)
        {
            bresenham_line4(0, 0, WQHG, i);
        }
    }
    mTimer.MomentEnd();
    for (size_t i = 0; i < mTimer.MomentCount; ++i)
    {
        std::cout << mTimer.mMomentNameS[i] << " : " << mTimer.mMomentTimerS[i] << std::endl;
    }
#endif
    // while (1){}
    return 0;
}