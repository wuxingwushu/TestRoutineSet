#pragma once

template<typename T>
class ContinuousPlate
{
	typedef void (*_GenerateCallback)(T* mT, int x, int y, void* Data);//生成回调函数
	typedef void (*_DeleteCallback)(T* mT, void* Data);//销毁回调函数
public:
	//创建连续板块，板块大小 X，Y， 板块间距 Edge
	ContinuousPlate(unsigned int x, unsigned int y, unsigned int edge) {
		mNumberX = x;
		mNumberY = y;
		mEdge = edge;
		mPlate = new T*[mNumberX];
		for (size_t i = 0; i < mNumberX; i++)
		{
			mPlate[i] = new T[mNumberY];
		}

		mPlateQueueX = new T*[mNumberX];
		mPlateQueueY = new T[mNumberY];
	}

	//设置起始位置
	void SetPos(float x, float y) {
		mX = int(x) / mEdge;
		mY = int(y) / mEdge;
	}

	//设置回调函数
	void SetCallback(_GenerateCallback G, void* GData, _DeleteCallback D, void* DData) {
		GenerateCallback = G;
		GenerateData = GData;
		DeleteCallback = D;
		DeleteData = DData;
	}

	//这种连续板块的中心
	void SetOrigin(unsigned int x, unsigned int y) {
		mOriginX = x;
		mOriginY = y;
	}

	//析构
	~ContinuousPlate() {
		for (size_t i = 0; i < mNumberX; i++)
		{
			delete mPlate[i];
		}
		delete mPlate;
		delete mPlateQueueX;
		delete mPlateQueueY;
	}

	//获取那个板块
	T* GetPlate(unsigned int x, unsigned int y) {
		return &mPlate[x][y];
	}

	//更新当前监测位置，判断是否需要移动板块，返回板块是否移动信息
	bool UpData(float x, float y) {
		int uX = mX - (int(x) / mEdge);
		int uY = mY - (int(y) / mEdge);
		bool UpDataBool = false;
		if ((fabs(uX) < mNumberX) && (fabs(uY) < mNumberY)) {
			if (uX != 0)
			{
				MovePlateX(uX);
				mX = (int(x) / mEdge);
				UpDataBool = true;
			}
			if (uY != 0)
			{
				MovePlateY(uY);
				mY = (int(y) / mEdge);
				UpDataBool = true;
			}
		}
		else
		{
			mX = (int(x) / mEdge);
			mY = (int(y) / mEdge);
			for (size_t Nx = 0; Nx < mNumberX; Nx++)
			{
				for (size_t Ny = 0; Ny < mNumberY; Ny++)
				{
					DeleteCallback(&mPlate[Nx][Ny], DeleteData);
					GenerateCallback(&mPlate[Nx][Ny], (mX + Nx - mOriginX), (mY + Ny - mOriginY), GenerateData);
				}
			}
			UpDataBool = true;
		}
		return UpDataBool;
	}

	//板块 X 的移动
	void MovePlateX(int uX) {
		if (uX > 0)
		{
			for (size_t i = 0; i < uX; i++)
			{
				mPlateQueueX[i] = mPlate[i];
				for (size_t y = 0; y < mNumberY; y++)
				{
					DeleteCallback(&mPlate[i][y], DeleteData);
				}
			}
			memcpy(mPlate, &mPlate[uX], (mNumberX - uX));
			for (size_t i = 0; i < uX; i++)
			{
				mPlate[mNumberX - uX + i] = mPlateQueueX[i];
				//mPlate[mNumberX - 1 - i] = mPlateQueueX[i];//没有顺序
				for (size_t y = 0; y < mNumberY; y++)
				{
					GenerateCallback(&mPlate[mNumberX - uX + i][y], (mNumberX - uX + i) - mOriginX, y - mOriginY, GenerateData);
				}
			}
		}
		else
		{
			for (int i = 0; i < -uX; i++)
			{
				mPlateQueueX[i] = mPlate[mNumberX - i - 1];
				for (size_t y = 0; y < mNumberY; y++)
				{
					DeleteCallback(&mPlate[mNumberX - i - 1][y], DeleteData);
				}
			}
			for (size_t i = (mNumberX - 1); i >= -uX; i--)
			{
				mPlate[i] = mPlate[i + uX];
			}
			for (size_t i = 0; i < -uX; i++)
			{
				mPlate[i] = mPlateQueueX[- i - uX - 1];
				for (size_t y = 0; y < mNumberY; y++)
				{
					GenerateCallback(&mPlate[i][y], i - mOriginX, y - mOriginY, GenerateData);
				}
			}
		}
	}

	//板块 Y 的移动
	void MovePlateY(int uY) {
		if (uY > 0)
		{
			for (size_t x = 0; x < mNumberX; x++)
			{
				for (size_t i = 0; i < uY; i++)
				{
					mPlateQueueY[i] = mPlate[x][i];
					DeleteCallback(&mPlate[x][i], DeleteData);
				}
				memcpy(mPlate[x], &mPlate[x][uY], (mNumberY - uY));
				for (size_t i = 0; i < uY; i++)
				{
					mPlate[x][mNumberY - uY + i] = mPlateQueueY[i];
					GenerateCallback(&mPlate[x][mNumberY - uY + i], x - mOriginX, (mNumberY - uY + i - mOriginY), GenerateData);
					//mPlate[x][mNumberY - 1 - i] = mPlateQueueY[i];//没有顺序
				}
			}
		}
		else
		{
			for (size_t x = 0; x < mNumberX; x++)
			{
				for (int i = 0; i < -uY; i++)
				{
					mPlateQueueY[i] = mPlate[x][mNumberY - i - 1];
					DeleteCallback(&mPlate[x][mNumberY - i - 1], DeleteData);
				}
				for (size_t i = (mNumberY - 1); i >= -uY; i--)
				{
					mPlate[x][i] = mPlate[x][i + uY];
				}
				for (size_t i = 0; i < -uY; i++)
				{
					mPlate[x][i] = mPlateQueueY[-i - uY - 1];
					GenerateCallback(&mPlate[x][i], x - mOriginX, i - mOriginY, GenerateData);
				}
			}
		}
	}


private:
	int mX = 0, mY = 0;//当前位置
	unsigned int mEdge;//移动多少距离，板块跟着移动
	unsigned int mNumberX, mNumberY;//
	unsigned int mOriginX = 0, mOriginY = 0;//将那个板块设为原点
	T** mPlate;//板块

	_GenerateCallback GenerateCallback = nullptr;//生成回调函数
	void* GenerateData = nullptr;
	_DeleteCallback DeleteCallback = nullptr;//销毁回调函数
	void* DeleteData = nullptr;

private://队列
	T** mPlateQueueX;
	T* mPlateQueueY;
};