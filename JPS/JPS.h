#pragma once

#define JPS_MemoryPool
//#include <set>
#ifdef JPS_MemoryPool// 开启  内存池
#include "MemoryPool.h"
#endif
#include <unordered_set>


enum class JPSDirection {
    Upper = 0,      //上
    Lower,          //下
    Left,           //左
    Right,          //右
    UpperLeft,      //左上
    LowerLeft,      //左下
    UpperRight,     //右上
    LowerRight      //右下
};

struct JPSNode {
    int x;
    int y;
    float g;    // 累积代价
    float h;    // 启发式代价
    float f;    // 总代价
    JPSNode* parent = nullptr;    //父节点
    std::vector<JPSNode*> pChild{}; //子节点
    JPSDirection Direction;
    JPSNode(int _x, int _y, float _g, float _h, JPSNode* _parent, JPSDirection direction) : x(_x), y(_y), g(_g), h(_h), parent(_parent), Direction(direction){
        f = g + h;
    }

    ~JPSNode()
    {
    }
};

struct JPSVec2 {
    int x;
    int y;

    constexpr JPSVec2& operator+=(const JPSVec2& other) noexcept
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr JPSVec2 operator+(const JPSVec2& other) noexcept
    {
        return { x += other.x, y += other.y };
    }

    constexpr JPSVec2& operator-=(const JPSVec2& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr JPSVec2 operator-(const JPSVec2& other) noexcept
    {
        return { x - other.x, y - other.y };
    }

    constexpr bool operator!=(const JPSVec2& other) noexcept
    {
        return ((x != other.x) || (y != other.y));
    }
};

struct NodeData
{
    int x;
    int y;
    JPSDirection Direction;

    NodeData(int X, int Y, JPSDirection direction):x(X),y(Y), Direction(direction){}

    bool operator<(const NodeData& other) const {
        return x < other.x || (x == other.x && y < other.y) || (x == other.x && y == other.y && Direction < other.Direction);
    }

    bool operator==(const NodeData& other) const {
        return x == other.x && y == other.y && Direction == other.Direction;
    }
};

struct NodeDataHash {
    std::size_t operator()(const NodeData& node) const {
        std::size_t h1 = std::hash<int>{}(node.x);
        std::size_t h2 = std::hash<int>{}(node.y);
        std::size_t h3 = std::hash<int>{}(static_cast<int>(node.Direction));
        return h3 + (h2 << 1) + (h1 << 2);
    }
};



class JPS {
private:
    //std::set<NodeData> RepeatJudge;     //有过的跳点
    std::unordered_set<NodeData, NodeDataHash> RepeatJudge;     //有过的跳点
private:
    JPSVec2 TargetPosition{};           //目标位置
    JPSVec2 CurrentPosition{};          //当前位置
    JPSNode* ParentJPSNode = nullptr;   //当前节点
    bool Finish = false;                //寻路是否结束
    bool Scanning = true;               //是否是初次跳点
private:
    JPSVec2 StartingPoint = { 0,0 };                //开始位置（也是偏移值）
    const int mRange = 0;                           //范围
    const int mSteps = 0;                           //最大步数
    bool mPathfindingCompleted = true;              //寻路是否结束
    typedef bool (*_ObstaclesCallback)(int x, int y, void* ptr);
    _ObstaclesCallback mObstaclesCallback = nullptr;    //是否为障碍物回调函数
    void* mDataPointer = nullptr;                       //回调参数

    std::vector<JPSNode*>* mJPSDirectionVec2;       //子集指针
#ifdef JPS_MemoryPool
    MemoryPool<JPSNode, sizeof(JPSNode) * 100> mMemoryPool;    //内存池
#endif

    //位置是否合法
    bool Legitimacy(int x, int y) {
        if (x >= -mRange && x < mRange && y >= -mRange && y < mRange) { return true; }
        else { return false; }
    }
    bool Legitimacy2(int x, int y) {
        if (x >= 0 && x < mRange * 2 && y >= 0 && y < mRange * 2) { return true; }
        else { return false; }
    }

    //调用回调函数
    bool isValid(int x, int y) {
        if (Legitimacy(x, y)) {
            return mObstaclesCallback(x + StartingPoint.x, y + StartingPoint.y, mDataPointer);
        }
        return false;
    }

    //计算代价
    double calculateH(int x, int y, int targetX, int targetY) {
        return fabs(x - targetX) + fabs(y - targetY);
    }

    //创建JPSNode
    JPSNode* NewJPSNode(int _x, int _y, float _g, float _h, JPSNode* _parent, JPSDirection direction) { 
#ifdef AStar_MemoryPool
        return mMemoryPool.newElement(_x, _y, _g, _h, _parent, direction);
#else
        return new JPSNode(_x, _y, _g, _h, _parent, direction);
#endif
    }

    //销毁JPSNode
    void DeleteJPSNode(JPSNode* _parent) {
#ifdef AStar_MemoryPool
        mMemoryPool.deleteElement(_parent);
#else
        delete _parent;
#endif
    }

    //判断是否到终点
    bool EndPointJudge(JPSVec2 pos) {
        bool BJ = pos.x == TargetPosition.x && pos.y == TargetPosition.y;
        if (BJ) {
            Finish = true;
        }
        return BJ;
    }

    //方向的偏移
    JPSVec2 GetProgramme(JPSDirection Direction) {
        switch (Direction)
        {
        case JPSDirection::Upper:
            return JPSVec2{ 0, 1 };
            break;
        case JPSDirection::Lower:
            return JPSVec2{ 0, -1 };
            break;
        case JPSDirection::Left:
            return JPSVec2{ -1, 0 };
            break;
        case JPSDirection::Right:
            return JPSVec2{ 1, 0 };
            break;
        case JPSDirection::UpperLeft:
            return JPSVec2{ -1, 1 };
            break;
        case JPSDirection::LowerLeft:
            return JPSVec2{ -1, -1 };
            break;
        case JPSDirection::UpperRight:
            return JPSVec2{ 1, 1 };
            break;
        case JPSDirection::LowerRight:
            return JPSVec2{ 1, -1 };
            break;
        default:
            break;
        }
    }

    //获取合法位置
    bool GetDirectionLegitimate(JPSVec2 pos, JPSDirection Direction) {
        switch (Direction)
        {
        case JPSDirection::Upper:
            return isValid(pos.x, pos.y + 1);
            break;
        case JPSDirection::Lower:
            return isValid(pos.x, pos.y - 1);
            break;
        case JPSDirection::Left:
            return isValid(pos.x - 1, pos.y);
            break;
        case JPSDirection::Right:
            return isValid(pos.x + 1, pos.y);
            break;
        case JPSDirection::UpperLeft:
            return (isValid(pos.x - 1, pos.y) || isValid(pos.x, pos.y + 1)) && isValid(pos.x - 1, pos.y + 1);
            break;
        case JPSDirection::LowerLeft:
            return (isValid(pos.x - 1, pos.y) || isValid(pos.x, pos.y - 1)) && isValid(pos.x - 1, pos.y - 1);
            break;
        case JPSDirection::UpperRight:
            return (isValid(pos.x + 1, pos.y) || isValid(pos.x, pos.y + 1)) && isValid(pos.x + 1, pos.y + 1);
            break;
        case JPSDirection::LowerRight:
            return (isValid(pos.x + 1, pos.y) || isValid(pos.x, pos.y - 1)) && isValid(pos.x + 1, pos.y - 1);
            break;
        default:
            break;
        }
    }

    //邻近点的检测
    void RadialEvent(JPSVec2 pos, JPSNode* Parent, JPSDirection Direction) {
        Scanning = true;
        JPSNode* wParent = Parent;
        switch (Direction)
        {
        case JPSDirection::Upper:
            if(isValid(pos.x - 1, pos.y))RadialNearLeft({ pos.x - 1, pos.y }, wParent);
            Scanning = true;
            if (isValid(pos.x + 1, pos.y))RadialNearRight({ pos.x + 1, pos.y }, wParent);
            break;
        case JPSDirection::Lower:
            if (isValid(pos.x - 1, pos.y))RadialNearLeft({ pos.x - 1, pos.y }, wParent);
            Scanning = true;
            if (isValid(pos.x + 1, pos.y))RadialNearRight({ pos.x + 1, pos.y }, wParent);
            break;
        case JPSDirection::Left:
            if (isValid(pos.x, pos.y + 1))RadialNearUpper({ pos.x, pos.y + 1 }, wParent);
            Scanning = true;
            if (isValid(pos.x, pos.y - 1))RadialNearLower({ pos.x, pos.y - 1 }, wParent);
            break;
        case JPSDirection::Right:
            if (isValid(pos.x, pos.y + 1))RadialNearUpper({ pos.x, pos.y + 1 }, wParent);
            Scanning = true;
            if (isValid(pos.x, pos.y - 1))RadialNearLower({ pos.x, pos.y - 1 }, wParent);
            break;
        case JPSDirection::UpperLeft:
            if (isValid(pos.x, pos.y + 1))RadialNearUpper({ pos.x, pos.y + 1 }, wParent);
            Scanning = true;
            if (isValid(pos.x - 1, pos.y))RadialNearLeft({ pos.x - 1, pos.y }, wParent);
            break;
        case JPSDirection::LowerLeft:
            if (isValid(pos.x, pos.y - 1))RadialNearLower({ pos.x, pos.y - 1 }, wParent);
            Scanning = true;
            if (isValid(pos.x - 1, pos.y))RadialNearLeft({ pos.x - 1, pos.y }, wParent);
            break;
        case JPSDirection::UpperRight:
            if (isValid(pos.x, pos.y + 1))RadialNearUpper({ pos.x, pos.y + 1 }, wParent);
            Scanning = true;
            if (isValid(pos.x + 1, pos.y))RadialNearRight({ pos.x + 1, pos.y }, wParent);
            break;
        case JPSDirection::LowerRight:
            if (isValid(pos.x, pos.y - 1))RadialNearLower({ pos.x, pos.y - 1 }, wParent);
            Scanning = true;
            if (isValid(pos.x + 1, pos.y))RadialNearRight({ pos.x + 1, pos.y }, wParent);
            break;
        default:
            break;
        }
    }

    //移动事件和邻近点的检测
    void MoveEvent(JPSVec2 pos, JPSNode* Parent, JPSDirection Direction) {
        Scanning = true;
        JPSNode* wParent = Parent;
        switch (Direction)
        {
        case JPSDirection::Upper:
            CurrentPosition = { pos.x, pos.y + 1 };
            RadialNearLeft(CurrentPosition, wParent);
            Scanning = true;
            RadialNearRight(CurrentPosition, wParent);
            break;
        case JPSDirection::Lower:
            CurrentPosition = { pos.x, pos.y - 1 };
            RadialNearLeft(CurrentPosition, wParent);
            Scanning = true;
            RadialNearRight(CurrentPosition, wParent);
            break;
        case JPSDirection::Left:
            CurrentPosition = { pos.x - 1, pos.y };
            RadialNearUpper(CurrentPosition, wParent);
            Scanning = true;
            RadialNearLower(CurrentPosition, wParent);
            break;
        case JPSDirection::Right:
            CurrentPosition = { pos.x + 1, pos.y };
            RadialNearUpper(CurrentPosition, wParent);
            Scanning = true;
            RadialNearLower(CurrentPosition, wParent);
            break;
        case JPSDirection::UpperLeft:
            CurrentPosition = { pos.x - 1, pos.y + 1 };
            RadialNearUpper(CurrentPosition, wParent);
            Scanning = true;
            RadialNearLeft(CurrentPosition, wParent);
            break;
        case JPSDirection::LowerLeft:
            CurrentPosition = { pos.x - 1, pos.y - 1 };
            RadialNearLower(CurrentPosition, wParent);
            Scanning = true;
            RadialNearLeft(CurrentPosition, wParent);
            break;
        case JPSDirection::UpperRight:
            CurrentPosition = { pos.x + 1, pos.y + 1 };
            RadialNearUpper(CurrentPosition, wParent);
            Scanning = true;
            RadialNearRight(CurrentPosition, wParent);
            break;
        case JPSDirection::LowerRight:
            CurrentPosition = { pos.x + 1, pos.y - 1 };
            RadialNearLower(CurrentPosition, wParent);
            Scanning = true;
            RadialNearRight(CurrentPosition, wParent);
            break;
        default:
            break;
        }
        if (EndPointJudge(CurrentPosition)) {
            JPSNode* jiao = NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.414f, calculateH(CurrentPosition.x, CurrentPosition.y, TargetPosition.x, TargetPosition.y), wParent, wParent->Direction);
            ParentJPSNode = jiao;
        }
    }

    //添加邻近点
    JPSNode* AddSkipPoint(JPSVec2 posd, JPSNode* ParentD, std::vector<JPSNode*> Skip, JPSDirection Direction) {
        if (Skip.size() != 0 && Scanning) {
            Scanning = false;
            JPSVec2 LSpos = CurrentPosition - GetProgramme(ParentD->Direction);
            JPSNode* jiao = NewJPSNode(LSpos.x, LSpos.y, 1.414f, calculateH(LSpos.x, LSpos.y, TargetPosition.x, TargetPosition.y), ParentD, ParentD->Direction);
            ParentD->pChild.push_back(jiao);
            JPSVec2 LSpos2 = LSpos - posd;
            if (fabs(LSpos2.x) > 1 || fabs(LSpos2.y) > 1) {
                if (fabs(LSpos2.x) > 1) {
                    ParentD = NewJPSNode(LSpos.x + (LSpos2.x < 0 ? 1 : -1), posd.y, 1.414f, calculateH(LSpos.x + (LSpos2.x < 0 ? 1 : -1), posd.y, TargetPosition.x, TargetPosition.y), jiao, Direction);
                }
                else {
                    ParentD = NewJPSNode(posd.x, LSpos.y + (LSpos2.y < 0 ? 1 : -1), 1.414f, calculateH(posd.x, LSpos.y + (LSpos2.y < 0 ? 1 : -1), TargetPosition.x, TargetPosition.y), jiao, Direction);
                }
            }
            else {
                ParentD = NewJPSNode(posd.x, posd.y, 1.414f, calculateH(posd.x, posd.y, TargetPosition.x, TargetPosition.y), jiao, Direction);
            }
            jiao->pChild.push_back(ParentD);
            mJPSDirectionVec2 = &ParentD->pChild;
            for (auto i : Skip)
            {
                i->parent = ParentD;
                mJPSDirectionVec2->push_back(i);
            }
        }
        else {
            for (auto i : Skip)
            {
                mJPSDirectionVec2->push_back(i);
            }
        }
        return ParentD;
    }


    //上
    void RadialNearUpper(JPSVec2 pos, JPSNode* Parent) {
        if (EndPointJudge(pos)) {
            JPSNode* jiao = NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.414f, calculateH(CurrentPosition.x, CurrentPosition.y, TargetPosition.x, TargetPosition.y), Parent, Parent->Direction);
            ParentJPSNode = NewJPSNode(pos.x, pos.y, 0, 0, jiao, JPSDirection::UpperLeft);
            return;
        }
        std::vector<JPSNode*> LSJPSDirectionVec2;
        if (!isValid(pos.x - 1, pos.y) && isValid(pos.x - 1, pos.y + 1))//左上强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::UpperLeft }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x - 1, pos.y + 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::UpperLeft));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::UpperLeft });
            }
        }
        if (!isValid(pos.x + 1, pos.y) && isValid(pos.x + 1, pos.y + 1))//右上强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::UpperRight }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x + 1, pos.y + 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::UpperRight));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::UpperRight });
            }
        }
        Parent = AddSkipPoint(pos, Parent, LSJPSDirectionVec2, JPSDirection::Upper);
        if (isValid(pos.x, pos.y + 1)) {
            RadialNearUpper({ pos.x, pos.y + 1 }, Parent);
        }
    }

    //下
    void RadialNearLower(JPSVec2 pos, JPSNode* Parent) {
        if (EndPointJudge(pos)) {
            JPSNode* jiao = NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.414f, calculateH(CurrentPosition.x, CurrentPosition.y, TargetPosition.x, TargetPosition.y), Parent, Parent->Direction);
            jiao = NewJPSNode(pos.x, pos.y, 0, 0, jiao, JPSDirection::UpperLeft);
            ParentJPSNode = jiao;
            return;
        }
        std::vector<JPSNode*> LSJPSDirectionVec2;
        if (!isValid(pos.x - 1, pos.y) && isValid(pos.x - 1, pos.y - 1))//左下强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::LowerLeft }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x - 1, pos.y - 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::LowerLeft));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::LowerLeft });
            }
        }
        if (!isValid(pos.x + 1, pos.y) && isValid(pos.x + 1, pos.y - 1))//右下强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::LowerRight }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x + 1, pos.y - 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::LowerRight));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::LowerRight });
            }
        }
        Parent = AddSkipPoint(pos, Parent, LSJPSDirectionVec2, JPSDirection::Lower);
        if (isValid(pos.x, pos.y - 1)) {
            RadialNearLower({ pos.x, pos.y - 1 }, Parent);
        }
    }

    //左
    void RadialNearLeft(JPSVec2 pos, JPSNode* Parent) {
        if (EndPointJudge(pos)) {
            JPSNode* jiao = NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.414f, calculateH(CurrentPosition.x, CurrentPosition.y, TargetPosition.x, TargetPosition.y), Parent, Parent->Direction);
            jiao = NewJPSNode(pos.x, pos.y, 0, 0, jiao, JPSDirection::UpperLeft);
            ParentJPSNode = jiao;
            return;
        }
        std::vector<JPSNode*> LSJPSDirectionVec2;
        if (!isValid(pos.x, pos.y - 1) && isValid(pos.x - 1, pos.y - 1))//左下强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::LowerLeft }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x - 1, pos.y - 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::LowerLeft));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::LowerLeft });
            }
        }
        if (!isValid(pos.x, pos.y + 1) && isValid(pos.x - 1, pos.y + 1))//左上强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::UpperLeft }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x - 1, pos.y + 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::UpperLeft));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::UpperLeft });
            }
        }
        Parent = AddSkipPoint(pos, Parent, LSJPSDirectionVec2, JPSDirection::Left);
        if (isValid(pos.x - 1, pos.y)) {
            RadialNearLeft({ pos.x - 1, pos.y }, Parent);
        }
    }

    //右
    void RadialNearRight(JPSVec2 pos, JPSNode* Parent) {
        if (EndPointJudge(pos)) {
            JPSNode* jiao = NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.414f, calculateH(CurrentPosition.x, CurrentPosition.y, TargetPosition.x, TargetPosition.y), Parent, Parent->Direction);
            jiao = NewJPSNode(pos.x, pos.y, 0, 0, jiao, JPSDirection::UpperLeft);
            ParentJPSNode = jiao;
            return;
        }
        std::vector<JPSNode*> LSJPSDirectionVec2;
        if (!isValid(pos.x, pos.y - 1) && isValid(pos.x + 1, pos.y - 1))//右下强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::LowerRight }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x + 1, pos.y - 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::LowerRight));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::LowerRight });
            }
        }
        if (!isValid(pos.x, pos.y + 1) && isValid(pos.x + 1, pos.y + 1))//右上强制邻近点
        {
            if (RepeatJudge.find({ pos.x, pos.y, JPSDirection::UpperRight }) == RepeatJudge.end()) {
                float newH = calculateH(pos.x + 1, pos.y + 1, TargetPosition.x, TargetPosition.y);
                LSJPSDirectionVec2.push_back(NewJPSNode(pos.x, pos.y, 1.414f, newH, Parent, JPSDirection::UpperRight));
                RepeatJudge.insert({ pos.x, pos.y, JPSDirection::UpperRight });
            }
        }
        Parent = AddSkipPoint(pos, Parent, LSJPSDirectionVec2, JPSDirection::Right);
        if (isValid(pos.x + 1, pos.y)) {
            RadialNearRight({ pos.x + 1, pos.y }, Parent);
        }
    }

    //销毁树
    void DeleteNodeS(JPSNode* jPSNode) {
        for (auto i : jPSNode->pChild)
        {
            if (i->pChild.size() != 0)DeleteNodeS(i);
            DeleteJPSNode(i);
        }
    }

    //节点排序
    void NodeSort(JPSNode* jPSNode) {
        std::sort(jPSNode->pChild.begin(), jPSNode->pChild.end(),
            [](JPSNode* a, JPSNode* b) {
                return a->f > b->f;
            }
        );
        for (auto i : jPSNode->pChild)
        {
            NodeSort(i);
        }
    }

public:
    JPS(int Range, int Steps) :mRange(Range), mSteps(Steps) {
    }
    ~JPS() {
    }

    //设置障碍物回调函数
    void SetObstaclesCallback(_ObstaclesCallback ObstaclesCallback, void* DataPointer) {
        mObstaclesCallback = ObstaclesCallback;
        mDataPointer = DataPointer;
    }

    //获取寻路是否结束
    bool GetPathfindingCompleted() {
        return mPathfindingCompleted;
    }

    //开始寻路
    void FindPath(JPSVec2 start, JPSVec2 target, std::vector<JPSVec2>* PathVector) {
        //开始寻路
        mPathfindingCompleted = false;
        Finish = false;
        StartingPoint = start;
        TargetPosition = target - start;
        CurrentPosition = { 0, 0 };
        RepeatJudge.clear();
        
        JPSNode* LSDEJPSNode = NewJPSNode(CurrentPosition.x, CurrentPosition.y, 0.0f, 0.0f, nullptr, JPSDirection::Upper);
        if (GetDirectionLegitimate(CurrentPosition, JPSDirection::Upper))LSDEJPSNode->pChild.push_back(NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.0f, calculateH(CurrentPosition.x, CurrentPosition.y + 1, TargetPosition.x, TargetPosition.y), LSDEJPSNode, JPSDirection::Upper));
        if (GetDirectionLegitimate(CurrentPosition, JPSDirection::Lower))LSDEJPSNode->pChild.push_back(NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.0f, calculateH(CurrentPosition.x, CurrentPosition.y - 1, TargetPosition.x, TargetPosition.y), LSDEJPSNode, JPSDirection::Lower));
        if (GetDirectionLegitimate(CurrentPosition, JPSDirection::Left ))LSDEJPSNode->pChild.push_back(NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.0f, calculateH(CurrentPosition.x - 1, CurrentPosition.y, TargetPosition.x, TargetPosition.y), LSDEJPSNode, JPSDirection::Left ));
        if (GetDirectionLegitimate(CurrentPosition, JPSDirection::Right))LSDEJPSNode->pChild.push_back(NewJPSNode(CurrentPosition.x, CurrentPosition.y, 1.0f, calculateH(CurrentPosition.x + 1, CurrentPosition.y, TargetPosition.x, TargetPosition.y), LSDEJPSNode, JPSDirection::Right));
        std::sort(LSDEJPSNode->pChild.begin(), LSDEJPSNode->pChild.end(),
            [](JPSNode* a, JPSNode* b) {
                return a->f > b->f;
            }
        );

        JPSNode* DeleteJPSNodeX;
        
        do
        {
            if (LSDEJPSNode->pChild.size() != 0) {
                DeleteJPSNodeX = LSDEJPSNode->pChild.back();
                LSDEJPSNode->pChild.pop_back();
                LSDEJPSNode = DeleteJPSNodeX;
                ParentJPSNode = LSDEJPSNode;//绑定父类

                mJPSDirectionVec2 = &LSDEJPSNode->pChild;
                CurrentPosition = { LSDEJPSNode->x, LSDEJPSNode->y };
                if (mJPSDirectionVec2->size() == 0) {
                    //RadialEvent(CurrentPosition, LSDEJPSNode, LSDEJPSNode->Direction);
                    while (GetDirectionLegitimate(CurrentPosition, LSDEJPSNode->Direction)) {
                        MoveEvent(CurrentPosition, LSDEJPSNode, LSDEJPSNode->Direction);
                    }

                    if (LSDEJPSNode->pChild.size() == 0) {
                        DeleteJPSNodeX = LSDEJPSNode;
                        LSDEJPSNode = LSDEJPSNode->parent;
                        DeleteJPSNode(DeleteJPSNodeX);
                    }
                    else {
                        NodeSort(LSDEJPSNode);
                    }
                }
                if (Finish) {
                    break;
                }
            }
            else {
                DeleteJPSNodeX = LSDEJPSNode;
                LSDEJPSNode = LSDEJPSNode->parent;
                DeleteJPSNode(DeleteJPSNodeX);
            }
        } while (LSDEJPSNode != nullptr);
        if (LSDEJPSNode != nullptr) {
            PathVector->push_back({ ParentJPSNode->x + StartingPoint.x, ParentJPSNode->y + StartingPoint.y });
            if (ParentJPSNode->parent->pChild.size() != 0) {
                if (ParentJPSNode->parent->pChild.back() != ParentJPSNode) {//和最后一个元素不重复
                    ParentJPSNode->parent->pChild.push_back(ParentJPSNode);//添加到多叉树
                }
            }
            else {
                ParentJPSNode->parent->pChild.push_back(ParentJPSNode);//添加到多叉树
            }
            ParentJPSNode = ParentJPSNode->parent;
            JPSVec2 CFpos;
            do
            {
                CFpos = JPSVec2{ ParentJPSNode->x + StartingPoint.x, ParentJPSNode->y + StartingPoint.y };
                if (PathVector->back() != CFpos) {
                    PathVector->push_back(CFpos);
                }
                if (ParentJPSNode->parent == nullptr) {
                    DeleteNodeS(ParentJPSNode);//销毁树
                    DeleteJPSNode(ParentJPSNode);
                    mPathfindingCompleted = true;
                    return;
                }
                else {
                    if (ParentJPSNode->parent->pChild.size() != 0) {
                        if (ParentJPSNode->parent->pChild.back() != ParentJPSNode) {//和最后一个元素不重复
                            ParentJPSNode->parent->pChild.push_back(ParentJPSNode);//添加到多叉树
                        }
                    }
                    else {
                        ParentJPSNode->parent->pChild.push_back(ParentJPSNode);//添加到多叉树
                    }
                }
                ParentJPSNode = ParentJPSNode->parent;
            } while (true);
        }

        mPathfindingCompleted = true;
        return;
    }
};