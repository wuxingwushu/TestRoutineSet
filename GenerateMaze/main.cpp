#include <iostream>

unsigned int BLOCK_HEIGHT, BLOCK_WIDTH;
bool** block;

struct Pos {
	int X;
	int Y;
};

template <typename T>
class PileUp
{
private:
	unsigned int Index = 0;
	unsigned int Max;
	T* mPileUp;
public:
	PileUp(unsigned int size) {
		Max = size;
		mPileUp = new T[size];
	};

	~PileUp() {
		delete mPileUp;
	};

	void add(T Parameter) {
		if (Index >= Max)
		{
			std::cout << "超出" << std::endl;
			return;
		}
		mPileUp[Index] = Parameter;
		Index++;
	};

	T pop() {
		if (Index == 0)
		{
			std::cout << "空" << std::endl;
			return 0;
		}
		Index--;
		return mPileUp[Index];
	}

	unsigned int GetNumber() {
		return Index;
	}

	T GetEnd() {
		return mPileUp[Index - 1];
	}

	void pop__() {
		Index--;
	}

	T GetIndex(unsigned int I) {
		return mPileUp[I];
	}
};

bool check(Pos pns) {
	//上
	if (pns.Y - 2 >= 0) {
		if (block[pns.X][pns.Y - 2] == true) {
			return 0;
		}
	}
	//下
	if (pns.Y + 2 <= BLOCK_HEIGHT - 1) {
		if (block[pns.X][pns.Y + 2] == true) {
			return 0;
		}
	}
	//左
	if (pns.X - 2 >= 0) {
		if (block[pns.X - 2][pns.Y] == true) {
			return 0;
		}
	}
	//右
	if (pns.X + 2 <= BLOCK_WIDTH - 1) {
		if (block[pns.X + 2][pns.Y] == true) {
			return 0;
		}
	}
	return 1;
}


void through(PileUp<Pos>* PUp) {
	PileUp<Pos> mPos1(4);
	PileUp<Pos> mPos2(4);
	Pos pns = PUp->GetEnd();
	//上
	if (pns.Y - 2 >= 0) {
		if (block[pns.X][pns.Y - 2] == true) {
			mPos1.add(Pos{ pns.X, pns.Y - 1 });
			mPos2.add(Pos{ pns.X, pns.Y - 2 });
		}
	}
	//下
	if (pns.Y + 2 <= BLOCK_HEIGHT - 1) {
		if (block[pns.X][pns.Y + 2] == true) {
			mPos1.add(Pos{ pns.X, pns.Y + 1 });
			mPos2.add(Pos{ pns.X, pns.Y + 2 });
		}
	}
	//左
	if (pns.X - 2 >= 0) {
		if (block[pns.X - 2][pns.Y] == true) {
			mPos1.add(Pos{ pns.X - 1, pns.Y });
			mPos2.add(Pos{ pns.X - 2, pns.Y });
		}
	}
	//右
	if (pns.X + 2 <= BLOCK_WIDTH - 1) {
		if (block[pns.X + 2][pns.Y] == true) {
			mPos1.add(Pos{ pns.X + 1, pns.Y });
			mPos2.add(Pos{ pns.X + 2, pns.Y });
		}
	}
	if (mPos1.GetNumber() != 0) {
		int BIndexea = rand() % mPos1.GetNumber();
		Pos B1 = mPos1.GetIndex(BIndexea);
		Pos B2 = mPos2.GetIndex(BIndexea);
		block[B1.X][B1.Y] = 0;
		block[B2.X][B2.Y] = 0;
		PUp->add(B2);
	}
}


void GenerateMaze(int X, int Y) {
	BLOCK_HEIGHT = Y;
	BLOCK_WIDTH = X;
	PileUp<Pos> mPos(X * Y);
	for (size_t x = 0; x < BLOCK_WIDTH; x++)
	{
		for (size_t y = 0; y < BLOCK_HEIGHT; y++)
		{
			block[x][y] = true;
		}
	}

	mPos.add(Pos{ 3, 3 });

	while (mPos.GetNumber() != 0)
	{
		while (check(mPos.GetEnd()))
		{

			mPos.pop__();
			if (mPos.GetNumber() == 0) {
				for (size_t x = 0; x < BLOCK_WIDTH; x++)
				{
					for (size_t y = 0; y < BLOCK_HEIGHT; y++)
					{
						if (block[x][y])
						{
							std::cout << "1 ";
						}
						else {
							std::cout << "  ";
						}
					}
					std::cout << std::endl;
				}
				return;
			}
		}
		through(&mPos);
	}
}



int main() {
	block = new bool* [41];
	for (int i = 0; i < 41; i++) {
		block[i] = new bool[41];
	}
	std::cout << "kaishi" << std::endl;
	GenerateMaze(41, 41);
}