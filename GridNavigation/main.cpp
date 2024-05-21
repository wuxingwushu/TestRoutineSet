#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <Windows.h>
#include <profileapi.h>
#include "GridNavigation.h"


std::vector<std::vector<int>> map = {
//   0  1  2  3  4  5  6  7  8  9  0
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 0
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 1
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 2
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 3
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 4
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 6
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 7
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 8
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// 9
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} // 0
};

bool GetMap(int x, int y, void* P) {
    if (x >= 0 && x < map.size() && y >= 0 && y < map[0].size()) {
        return map[x][y];
    }
    return true;
}


int main() {
    GridNavigation mG(map.size(), map[0].size(),3);
    mG.SetRoadCallback(GetMap, nullptr);
    mG.ScanGridNavigation();
    map[4][4] = 1;//添加
    mG.add(4,4);
    for (size_t x = 0; x < map.size(); x++)
    {
        for (size_t y = 0; y < map[0].size(); y++)
        {
            std::cout << mG.Get(x, y) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------------------" << std::endl;
    map[4][4] = 0;//移除
    mG.pop(4, 4);
    for (size_t x = 0; x < map.size(); x++)
    {
        for (size_t y = 0; y < map[0].size(); y++)
        {
            std::cout << mG.Get(x, y) << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
