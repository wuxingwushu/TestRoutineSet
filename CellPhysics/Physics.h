#include <Vector>
#include <cmath>

struct vec2
{
    float x;
    float y;

    constexpr vec2 operator+(const vec2 &other) noexcept
    {
        return {x + other.x, y + other.y};
    }

    constexpr vec2 operator-(const vec2 &other) noexcept
    {
        return {x - other.x, y - other.y};
    }

    constexpr vec2 operator*(const vec2 &other) noexcept
    {
        return {x * other.x, y * other.y};
    }

    constexpr vec2 operator*(const float &other) noexcept
    {
        return {x * other, y * other};
    }

    constexpr vec2 operator/(const vec2 &other) noexcept
    {
        return {x / other.x, y / other.y};
    }

    constexpr vec2 operator/(const float &other) noexcept
    {
        return {x / other, y / other};
    }
};

float Modulus(vec2 Modulus)
{
    return sqrt((Modulus.x * Modulus.x) + (Modulus.y * Modulus.y));
}

float EdgeVecToCosAngleFloat(vec2 XYedge)
{
    if (XYedge.x == 0)
    {
        if (XYedge.y == 0)
        {
            return 0; // 分母不可以为零
        }
        return XYedge.y < 0 ? -1.57f : 1.57f;
    }
    float BeveledEdge = acos(XYedge.x / Modulus(XYedge));
    return XYedge.y < 0 ? -BeveledEdge : BeveledEdge;
}

class cell
{
private:
public:
    float m;         // 質量
    vec2 v = {0, 0}; // 速度
    vec2 pos;        // 位置
    vec2 F = {0, 0}; // 受力

    cell(float inM, vec2 inpos) : m(inM), pos(inpos) {};
    ~cell() {};
};

class Lin
{
private:
    cell *cell1;
    cell *cell2;
    float Lval;

public:
    Lin(cell *c1, cell *c2, float Lc) : cell1(c1), cell2(c2), Lval(Lc) {};
    Lin(cell *c1, cell *c2) : cell1(c1), cell2(c2){
        Lval = Modulus(cell1->pos - cell2->pos);
    };
    ~Lin() {};

    void LinTime(float Time)
    {
        vec2 F = cell1->pos - cell2->pos;
        double L = Modulus(F);
        F = F / L * 10;
        L -= Lval;

        L = L * L * L;
        
        cell1->F = cell1->F - (F * L);
        cell2->F = cell2->F + (F * L);
    }

    vec2 pos1(){
        return cell1->pos;
    }

    vec2 pos2(){
        return cell2->pos;
    }
};

class Physics
{
private:
    vec2 zl; // 重力加速度
public:
    Physics(vec2 zlval) : zl(zlval) {};
    ~Physics() {};

    std::vector<cell *> CellS;
    std::vector<Lin *> LinS;

    void PhysicsTime(float time)
    {
        for (auto i : LinS)
        {
            i->LinTime(time);
        }
        for (auto i : CellS)
        {
            i->v = i->v + (i->F * time);
            i->F = zl;
            i->pos = i->pos + (i->v * time);

            i->v.y = i->v.y * 0.9985;
            if (i->pos.x < 0)
            {
                i->pos.x = 0;
                i->v.x = -i->v.x * 0.7;
            }
        }
    }
};
