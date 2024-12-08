#include <iostream>
#include "View.h"
#include "ViewPos.h"

float PI(float angle)
{
    return angle / 180 * M_PI;
}

int main()
{
    float ViewAngleX = PI(10);
    float ViewAngleY = PI(0);
    float ViewX = PI(110);
    float ViewY = PI(110);

    View *mView = new View(ViewX, ViewY, 5);

    std::vector<vec2> data = mView->GetViewBoox(ViewAngleX);
    std::cout << " ++++++++++++++++++++++++++++++ " << std::endl;
    for (auto i : data)
    {
        std::cout << i.x << " - " << i.y << std::endl;
    }
    std::cout << " ViewPos ++++++++++++++++++++++++++++++ " << std::endl;
    ViewPos* mViewPos = new ViewPos(ViewX, ViewY, 5);
    std::vector<ivec3> data3 = mViewPos->GetViewBox(ViewAngleX,ViewAngleY);
    for (auto i : data3)
    {
        std::cout << i.x << " - " << i.y << " - " << i.z << std::endl;
    }
    

    return 0;
}
