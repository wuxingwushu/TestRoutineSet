#include <windows.h>
#include <iostream>
#include <time.h>



LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMag, WPARAM wParam, LPARAM lParam);

int image_width = 300;
int image_height = 200;
unsigned int *ImgData;
bool *CellS;
bool ShowCell = false;
bool CellEr = false;
unsigned int &at(int x, int y)
{
    return ImgData[x * image_height + y];
}

bool &CellAt(unsigned int x, unsigned int y, unsigned char C)
{
    if (x >= image_width)
    {
        return CellEr;
    }
    if (y >= image_height)
    {
        return CellEr;
    }
    return CellS[x * image_height + y + (C * image_width * image_height)];
}

bool JSCell(unsigned int x, unsigned int y)
{
    unsigned char Num = 0;
    if (CellAt(x + 0, y + 1, ShowCell))
    {
        ++Num;
    }
    if (CellAt(x + 0, y - 1, ShowCell))
    {
        ++Num;
    }
    if (CellAt(x + 1, y + 1, ShowCell))
    {
        ++Num;
    }
    if (CellAt(x + 1, y - 1, ShowCell))
    {
        ++Num;
    }
    if (CellAt(x - 1, y + 1, ShowCell))
    {
        ++Num;
    }
    if (CellAt(x - 1, y - 1, ShowCell))
    {
        ++Num;
    }
    if (CellAt(x + 1, y + 0, ShowCell))
    {
        ++Num;
    }
    if (CellAt(x - 1, y + 0, ShowCell))
    {
        ++Num;
    }

    if (Num == 3)
    {
        return true;
    }
    else if (Num == 2)
    {
        return CellAt(x, y, ShowCell);
    }
    else
    {
        return false;
    }
}

int int_color(double r, double g, double b)
{
    // 将每个颜色分量的转换后的[0,255]值写入输出流
    return (int(255.999 * r) << 0) + (int(255.999 * g) << 8) + (int(255.999 * b) << 16);
}

void CellLoos()
{
    for (size_t x = 0; x < image_width; x++)
    {
        for (size_t y = 0; y < image_height; y++)
        {
            CellAt(x, y, !ShowCell) = JSCell(x, y);
            if (CellAt(x, y, !ShowCell))
            {
                at(x, y) = int_color(1, 1, 1);
            }
            else
            {
                at(x, y) = int_color(0, 0, 0);
            }
        }
    }
    ShowCell = !ShowCell;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    srand((unsigned int)(time(NULL)));

    ImgData = new unsigned int[image_width * image_height];
    CellS = new bool[2 * image_width * image_height];
    for (size_t i = 0; i < (image_width * image_height); ++i)
    {
        CellS[i] = rand() % 2;
    }
    CellLoos();

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
    SetTimer(hwnd, 99999, 500, NULL);

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
        HDC hdc = BeginPaint(hwnd, &ps);
        // HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        // FillRect(hdc, &ps.rcPaint, hBrush);

        for (size_t x = 0; x < image_width; ++x)
        {
            for (size_t y = 0; y < image_height; ++y)
            {
                SetPixel(hdc, x, y, at(x, y));
            }
        }

        EndPaint(hwnd, &ps);
        CellLoos();
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
