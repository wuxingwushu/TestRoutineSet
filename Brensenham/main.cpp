#include <graphics.h>
#pragma comment (lib,"EasyXa.lib")
#include <iostream>
using namespace std;

void bresenham_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    while (true) {
        putpixel(x0, y0, WHITE); // 将当前点着色
        if (x0 == x1 && y0 == y1) break; // 线条绘制完毕
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (x0 == x1 && y0 == y1) {
            putpixel(x0, y0, WHITE);
            break;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

int main() {
    initgraph(500, 500); // 初始化绘图窗口
    double x,y;
    for (size_t i = 0; i < 900; i++)
    {
        x = int(cos(double(i * 0.1f) / 180.0f * 3.14f) * 250) + 250;
        y = int(sin(double(i * 0.1f) / 180.0f * 3.14f) * 250) + 250;
        bresenham_line(250, 250, x, y); // 绘制线条
    }
    while (true)
    {

    }
    closegraph(); // 关闭绘图窗口
    return 0;
}

