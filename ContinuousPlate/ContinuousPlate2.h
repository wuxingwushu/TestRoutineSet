#pragma once

template<typename T>
class ContinuousPlate22
{
	typedef void (*_GenerateCallback)(T* mT, int x, int y, void* Data);//生成回调函数
	typedef void (*_DeleteCallback)(T* mT, void* Data);//销毁回调函数
public:
	//创建连续板块，板块大小 X，Y， 板块间距 Edge
	ContinuousPlate22(unsigned int x, unsigned int y, unsigned int edge) {
		mNumberX = x;
		mNumberY = y;
		mEdge = edge;
		mPlate = new T * [mNumberX];
		for (size_t i = 0; i < mNumberX; i++)
		{
			mPlate[i] = new T[mNumberY];
		}
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
	~ContinuousPlate22() {
		for (size_t i = 0; i < mNumberX; i++)
		{
			delete mPlate[i];
		}
		delete mPlate;
	}

	//获取那个板块
	T* GetPlate(unsigned int x, unsigned int y) {
		int MX = moveX + x;
		int MY = moveY + y;
		if (MX >= mNumberX)
		{
			MX -= mNumberX;
		}
		if (MY >= mNumberY)
		{
			MY -= mNumberY;
		}
		return &mPlate[MX][MY];
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
			
			int MX;
			int MY;
			for (size_t x = 0; x < uX; x++)
			{
				for (size_t y = 0; y < mNumberY; y++)
				{
					MX = moveX + x;
					MY = moveY + y;
					if (MX >= mNumberX)
					{
						MX -= mNumberX;
					}
					if (MY >= mNumberY)
					{
						MY -= mNumberY;
					}
					DeleteCallback(&mPlate[MX][MY], DeleteData);
					GenerateCallback(&mPlate[MX][MY], (MX + mX - mOriginX), (MY + mY - mOriginY), GenerateData);
				}
			}
			moveX += uX;
			if (moveX >= mNumberX) {
				moveX -= mNumberX;
			}
		}
		else
		{
			
			int MX;
			int MY;
			for (size_t x = (mNumberX + uX); x < mNumberX; x++)
			{
				for (size_t y = 0; y < mNumberY; y++)
				{
					MX = moveX + x;
					MY = moveY + y;
					if (MX >= mNumberX)
					{
						MX -= mNumberX;
					}
					if (MY >= mNumberY)
					{
						MY -= mNumberY;
					}
					DeleteCallback(&mPlate[MX][MY], DeleteData);
					GenerateCallback(&mPlate[MX][MY], (MX + mX - mOriginX), (MY + mY - mOriginY), GenerateData);
				}
			}
			moveX += uX;
			if (moveX < 0) {
				moveX += mNumberX;
			}
		}
	}

	//板块 Y 的移动
	void MovePlateY(int uY) {
		if (uY > 0)
		{
			
			int MX;
			int MY;
			for (size_t x = 0; x < mNumberX; x++)
			{
				for (size_t y = 0; y < uY; y++)
				{
					MX = moveX + x;
					MY = moveY + y;
					if (MX >= mNumberX)
					{
						MX -= mNumberX;
					}
					if (MY >= mNumberY)
					{
						MY -= mNumberY;
					}
					DeleteCallback(&mPlate[MX][MY], DeleteData);
					GenerateCallback(&mPlate[MX][MY], (MX + mX - mOriginX), (MY + mY - mOriginY), GenerateData);
				}
			}
			moveY += uY;
			if (moveY >= mNumberY) {
				moveY -= mNumberY;
			}
		}
		else
		{
			
			int MX;
			int MY;
			for (size_t x = 0; x < mNumberX; x++)
			{
				for (size_t y = mNumberY + uY; y < mNumberY; y++)
				{
					MX = moveX + x;
					MY = moveY + y;
					if (MX >= mNumberX)
					{
						MX -= mNumberX;
					}
					if (MY >= mNumberY)
					{
						MY -= mNumberY;
					}
					DeleteCallback(&mPlate[MX][MY], DeleteData);
					GenerateCallback(&mPlate[MX][MY], (MX + mX - mOriginX), (MY + mY - mOriginY), GenerateData);
				}
			}
			moveY += uY;
			if (moveY < 0) {
				moveY += mNumberY;
			}
		}
	}


private:
	int mX = 0, mY = 0;//当前位置
	unsigned int mEdge;//移动多少距离，板块跟着移动
	unsigned int mNumberX, mNumberY;//板块数量
	unsigned int mOriginX = 0, mOriginY = 0;//将那个板块设为原点
	T** mPlate;//板块

	int moveX = 0, moveY = 0;//板块移动

	_GenerateCallback GenerateCallback = nullptr;//生成回调函数
	void* GenerateData = nullptr;
	_DeleteCallback DeleteCallback = nullptr;//销毁回调函数
	void* DeleteData = nullptr;
};
