#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include "AStar.h"



std::vector<std::vector<bool>> map = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0}
};

bool GetMap(int x, int y, void* P) {
    if (x >= 0 && x < map.size() && y >= 0 && y < map[0].size()) {
        return !map[x][y];
    }
    return false;
}


int main() {
    AStarVec2 start = { 3, 2 };
    AStarVec2 target = { 0, 0 };
    AStar* astar = new AStar(6,100);
    std::vector<AStarVec2> path;
    astar->SetObstaclesCallback(GetMap, nullptr);

    astar->FindPath(start, target, &path);

    delete astar;

    std::cout << "路径：" << std::endl;
    for (int i = path.size() - 1; i >= 0; i--)
        std::cout << "(" << path[i].x << ", " << path[i].y << ")" << std::endl;

    return 0;
}
