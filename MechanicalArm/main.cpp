#include <iostream>
#include "ArmConsole.hpp"

class CameraRays
{
private:
	const double FocalLength;       // 焦距 mm
	const double SensorWidth;       // 传感器宽度 mm
	const double SensorHeigh;       // 传感器高度 mm
	const int StaticWidth;			// 静态宽度
	const int StaticHeigh;			// 静态高度
public:
	CameraRays(const double focalLength, const double sensorWidth, const double sensorHeigh, const unsigned int staticWidth, const unsigned int staticHeigh)
		:FocalLength(focalLength), SensorWidth(sensorWidth), SensorHeigh(sensorHeigh), StaticWidth(staticWidth), StaticHeigh(staticHeigh) {};
	~CameraRays() {};

	float WidthAngle(int Width, int W) {
		double Fw = double(W + ((StaticWidth - Width) / 2) - (StaticWidth / 2)) / (StaticWidth / 2);
		Fw *= SensorWidth;
		double L = sqrt(pow(FocalLength, 2) + pow(Fw, 2));
		double FwAngle = acos((pow(L, 2) + pow(FocalLength, 2) - pow(Fw, 2)) / (2 * L * FocalLength));
		return FwAngle * (Fw < 0 ? -1 : 1);
	}

	float HeighAngle(int Heigh, int H) {
		double Fh = double(H + ((StaticHeigh - Heigh) / 2) - (StaticHeigh / 2)) / (StaticHeigh / 2);
		Fh *= SensorHeigh;
		double L = sqrt(pow(FocalLength, 2) + pow(Fh, 2));
		double FhAngle = acos((pow(L, 2) + pow(FocalLength, 2) - pow(Fh, 2)) / (2 * L * FocalLength));
		return FhAngle * (Fh < 0 ? -1 : 1);
	}
};


int width = 1920;
int height = 1080;

double GetBianA(double A, double B, double AB) {
	double C_rad = M_PI - B - A;
	if (sin(C_rad) == 0)return 0;
	return AB * sin(A) / sin(C_rad);
}

double GetBianB(double A, double B, double AB) {
	double C_rad = M_PI - B - A;
	if (sin(C_rad) == 0)return 0;
	return AB * sin(B) / sin(C_rad);
}

// 计算视场角的函数
double calculateFieldOfView(double sensorSize, double focalLength) {
	double fov = 2 * atan(sensorSize / (2 * focalLength)) * (180 / M_PI); // 弧度转换为角度
	return fov;
}

int main() {
	// 定义传感器尺寸（假设以毫米为单位）和焦距（假设以毫米为单位）
	double sensorSize = 1 / 2.8; // 传感器尺寸（对角线），单位：毫米
	double focalLength = 3.95; // 焦距，单位：毫米

	// 计算视场角
	double fov = calculateFieldOfView(sensorSize, focalLength);

	// 输出结果
	std::cout << "视场角为：" << fov << " 度" << std::endl;

	CameraRays mCameraRays(3.95, 5.59, 3.14, 2304, 1296);

	Arm_State mState{ 1,7,7, 90 };
	ArmConsole mArmConsole(mState);
	mArmConsole.ArmActionFormula(mState);

	int BoxCoreX = width / 2 + 300, BoxCoreY = height / 2 - 100;

	const float MuBiaoZ = -3.5;
	Arm_Result Result = mArmConsole.GetResult();
	double HAngle = mCameraRays.HeighAngle(height, BoxCoreY);
	double WAngle = mCameraRays.WidthAngle(width, BoxCoreX);
	printf("CameraRays: %f, %f\n", HAngle / M_PI * 180, WAngle / M_PI * 180);
	double AngleX = (M_PI / 2) - Result.AngleOAB - Result.AngleABC - Result.AngleBCD;
	double AngleZ = (M_PI / 2) - Result.AngleTerritory;
	double Nx = 0, Ny = 0, Nz = 1;
	rotateX(AngleX + HAngle, Ny, Nz);
	rotateZ(AngleZ - WAngle, Nx, Ny);
	printf("ZZ: %f, %f, %f\n", Nx, Ny, Nz);
	printf("CameraAngle: %f, %f\n", (AngleX + HAngle) / M_PI * 180, (AngleZ - WAngle) / M_PI * 180);

	double Sx = 0, Sy = -1.5, Sz = -5;
	rotateX(AngleX, Sy, Sz);
	rotateZ(AngleZ, Sx, Sy);
	printf("CP: %f, %f, %f\n", Sx, Sy, Sz);
	double NGao = fabs(MuBiaoZ - mState.z - Sz);
	double BAngleX = -(AngleX + HAngle) - M_PI;
	double BH = GetBianA(BAngleX, M_PI / 2, NGao);
	double BHX = GetBianB(BAngleX, M_PI / 2, NGao);
	double BW = GetBianA(WAngle, M_PI / 2, BHX);
	rotateZ(AngleZ, BW, BH);
	


	Nx = NGao / Nz * Nx;
	Ny = NGao / Nz * Ny;
	//double BFB = b / -double(MuBiaoZ - mState.z - Sz);
	double BFB = (Ny * Ny) / (NGao * NGao) + 1;
	Nx = BFB * Nx;
	Nz = MuBiaoZ;
	printf("ZFT: %f, %f, %f\n", Nx, Ny, Nz);
	Ny += mState.y + Sy;
	Nx += mState.x + Sx;
	printf("MB: %f, %f, %f\n\n", Nx, Ny, Nz);

	return 0;
}
