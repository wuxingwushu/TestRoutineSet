#include <iostream>
#include "MovePlate.h"

void DC(int* Data, void* Class) {
	std::cout << "销毁：" << *Data << std::endl;
}

void GC(int* Data, int x, int y, void* Class) {
	std::cout << "生成：" << *Data << "  X: " << x << "  Y: " << y << std::endl;
}

int main() {
	MovePlate<int>* mContinuousPlate = new MovePlate<int>(4, 4, 1, 2, 2);//创建 4 * 4 的板块（板块间距为 1）
	mContinuousPlate->SetPos(0, 0);//设置起始位置
	mContinuousPlate->SetCallback(GC, nullptr, DC, nullptr);//设置回调函数
	for (size_t x = 0; x < 4; x++)
	{
		for (size_t y = 0; y < 4; y++)
		{
			*mContinuousPlate->GetPlate(x, y) = x + (y * 4);//填充板块
		}
	}


	auto F = [](MovePlate<int>* LContinuousPlate) {
		for (size_t x = 0; x < 4; x++)
		{
			for (size_t y = 0; y < 4; y++)
			{
				std::cout << *LContinuousPlate->GetPlate(x, y) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << "=======================" << std::endl;
		};
	F(mContinuousPlate);
	if (mContinuousPlate->UpData(0.1f, 1.0f)) {//更新检测位置
		F(mContinuousPlate);
	}
	if (mContinuousPlate->UpData(0.1f, 2.0f)) {//更新检测位置
		F(mContinuousPlate);
	}
	if (mContinuousPlate->UpData(0.1f, 3.0f)) {//更新检测位置
		F(mContinuousPlate);
	}
	if (mContinuousPlate->UpData(0.1f, 4.0f)) {//更新检测位置
		F(mContinuousPlate);
	}
	if (mContinuousPlate->UpData(0.1f, 5.0f)) {//更新检测位置
		F(mContinuousPlate);
	}
	if (mContinuousPlate->UpData(1.1f, 5.0f)) {//更新检测位置
		F(mContinuousPlate);
	}
	
	return 0;
}