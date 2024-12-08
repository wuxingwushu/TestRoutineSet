#include <vector>
#include <cmath>
#include <iostream>

struct ivec3
{
    int x;
    int y;
    int z;
};

struct vecP2
{
    float x;
    float y;
};

class ViewPos
{
private:
    float ViewX;
    float ViewY;
    int mR;

    //获取模的长度
	inline float Modulus(vecP2 modulus) {
		return sqrt((modulus.x * modulus.x) + (modulus.y * modulus.y));
	}

    //根据XY算出cos的角度
	inline float VecToAngle(vecP2 XYedge) {
		float BeveledEdge = Modulus(XYedge);
		if (BeveledEdge == 0) {
			return 0;//分母不可以为零
		}
		if (XYedge.x == 0) {
			return XYedge.y < 0 ? -1.57f : 1.57f;
		}
		BeveledEdge = acos(XYedge.x / BeveledEdge);
		if (XYedge.y < 0)
		{
			return -BeveledEdge;
		}
		return BeveledEdge;
	}

    bool IFBoox(ivec3 pos, float vX, float vY){
        float ViewAngle = VecToAngle({pos.x, pos.y});
        if(abs(ViewAngle - vX) > (ViewX / 2)){
            return false;
        }
        ViewAngle = VecToAngle({pos.z, Modulus({pos.x, pos.y})});
        if(abs(ViewAngle - vY) > (ViewY / 2)){
            return false;
        }
        return true;
    }

public:
    ViewPos(float viewX, float viewY, int r) : ViewX(viewX), ViewY(viewY), mR(r) {};
    ~ViewPos() {};

    std::vector<ivec3> GetViewBox(float vX, float vY){
        std::vector<ivec3> Data{};
        for (int x = -mR; x < mR; ++x)
        {
            for (int y = -mR; y < mR; ++y)
            {
                for (int z = -mR; z < mR; ++z)
                {
                    if(IFBoox({x, y, z}, vX, vY))Data.push_back({x, y, z});
                }
            }
        }
        return Data;
    }
};
