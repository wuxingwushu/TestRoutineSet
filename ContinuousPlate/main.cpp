#include <iostream>
#include "ContinuousPlate.h"
#include "ContinuousPlate2.h"

void DC(int* Data, void* Class) {
	std::cout << "销毁：" << *Data << std::endl;
}

void GC(int* Data, int x, int y, void* Class) {
	std::cout << "生成：" << *Data << "  X: " << x << "  Y: " << y << std::endl;
}

int main() {
	ContinuousPlate22<int>* mContinuousPlate = new ContinuousPlate22<int>(4, 4, 1);

	//ContinuousPlate<int>* mContinuousPlate = new ContinuousPlate<int>(4, 4, 1);//创建 4 * 4 的板块（板块间距为 1）
	mContinuousPlate->SetPos(0, 0);//设置起始位置
	mContinuousPlate->SetCallback(GC, nullptr, DC, nullptr);//设置回调函数
	mContinuousPlate->SetOrigin(2, 2);//设置板块的原点
	for (size_t x = 0; x < 4; x++)
	{
		for (size_t y = 0; y < 4; y++)
		{
			*mContinuousPlate->GetPlate(x, y) = x + (y * 4);//填充板块
		}
	}

	if (mContinuousPlate->UpData(0.1f, 1.0f)) {//更新检测位置
		std::cout << "需要更新地图" << std::endl;
	}
	if (mContinuousPlate->UpData(0.1f, 2.0f)) {//更新检测位置
		std::cout << "需要更新地图" << std::endl;
	}
	if (mContinuousPlate->UpData(0.1f, 3.0f)) {//更新检测位置
		std::cout << "需要更新地图" << std::endl;
	}
	if (mContinuousPlate->UpData(0.1f, 4.0f)) {//更新检测位置
		std::cout << "需要更新地图" << std::endl;
	}
	if (mContinuousPlate->UpData(0.1f, 5.0f)) {//更新检测位置
		std::cout << "需要更新地图" << std::endl;
	}
	
	return 0;
}