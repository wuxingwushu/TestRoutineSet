#include <windows.h>
#include <iostream>
#include <time.h>
#include "Physics.h"


LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMag, WPARAM wParam, LPARAM lParam);

int image_width = 300;
int image_height = 200;
unsigned int *ImgData;
bool hen = true;

unsigned int &at(int x, int y)
{
    return ImgData[x * image_height + y];
}


int int_color(double r, double g, double b)
{
    // 将每个颜色分量的转换后的[0,255]值写入输出流
    return (int(255.999 * r) << 0) + (int(255.999 * g) << 8) + (int(255.999 * b) << 16);
}

HDC hdc;
void bresenham_YB(int x0, int y0, int x1, int y1, int color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;
    while (true)
    {
        SetPixel(hdc, x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
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

std::vector<vec2> DianData(4);
Physics* mPhysics;
void init(){
    mPhysics = new Physics({-9.8, 0});
    mPhysics->CellS.push_back(new cell(10, {210, 0}));
    mPhysics->CellS.push_back(new cell(10, {220, 10}));
    mPhysics->CellS.push_back(new cell(10, {210, 20}));
    mPhysics->CellS.push_back(new cell(10, {200, 10}));
    mPhysics->LinS.push_back(new Lin(mPhysics->CellS[0], mPhysics->CellS[1]));
    mPhysics->LinS.push_back(new Lin(mPhysics->CellS[1], mPhysics->CellS[2]));
    mPhysics->LinS.push_back(new Lin(mPhysics->CellS[2], mPhysics->CellS[3]));
    mPhysics->LinS.push_back(new Lin(mPhysics->CellS[3], mPhysics->CellS[0]));
    mPhysics->LinS.push_back(new Lin(mPhysics->CellS[0], mPhysics->CellS[2]));
    mPhysics->LinS.push_back(new Lin(mPhysics->CellS[1], mPhysics->CellS[3]));
}

void Run(){
    for (auto i : mPhysics->CellS)
    {
        SetPixel(hdc, image_width / 2 + i->pos.y, image_height - 2 - i->pos.x, int_color(0,0,0));
    }
    for (auto i : mPhysics->LinS)
    {
        bresenham_YB(image_width / 2 + i->pos1().y, image_height - 2 - i->pos1().x, image_width / 2 + i->pos2().y, image_height - 2 - i->pos2().x, int_color(0,0,0));
    }
    for(int i = 0; i < 5; ++i){
        mPhysics->PhysicsTime(0.01);
    }
    for (auto i : mPhysics->CellS)
    {
        SetPixel(hdc, image_width / 2 + i->pos.y, image_height - 2 - i->pos.x, int_color(1,0,0));
    }
    for (auto i : mPhysics->LinS)
    {
        bresenham_YB(image_width / 2 + i->pos1().y, image_height - 2 - i->pos1().x, image_width / 2 + i->pos2().y, image_height - 2 - i->pos2().x, int_color(1,0,0));
    }
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    init();
    srand((unsigned int)(time(NULL)));

    ImgData = new unsigned int[image_width * image_height];
    for (size_t i = 0; i < (image_width * image_height); ++i)
    {
        ImgData[i] = 0;
    }

    const char name[] = "changko";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = name;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        name,
        "Show",
        WS_OVERLAPPEDWINDOW,
        960 - image_width / 2, 540 - image_height / 2 * 1.22f, image_width + 16 /* *1.055*/, image_height + 38 /* * 1.195*/,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL)
    {
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 99999 是標誌
    SetTimer(hwnd, 99999, 10, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMag, WPARAM wParam, LPARAM lParam)
{
    switch (uMag)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        if(hen){
            hen = false;
            for (size_t x = 0; x < image_width; x++)
            {
                for (size_t y = 0; y < image_height; y++)
                {
                    SetPixel(hdc, x, y, int_color(0,0,0));
                }
            }
        }
        Run();
        EndPaint(hwnd, &ps);
    }
        return 0;

    case WM_TIMER:
        if (wParam == 99999)
        {
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMag, wParam, lParam);
}
