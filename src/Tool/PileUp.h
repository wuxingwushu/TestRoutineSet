#pragma once
#include "Iterator.h"

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
            std::cout << "[PileUp]Error: GoBeyond" << std::endl;
            return;
        }
        //memcpy(&mPileUp[Index], &Parameter, sizeof(T));
        mPileUp[Index] = Parameter;
        ++Index;
    };

    [[nodiscard]] T* pop() {
        if (Index == 0)
        {
            std::cout << "[PileUp]Error: Empty" << std::endl;
            return nullptr;
        }
        --Index;
        return &mPileUp[Index];
    }

    [[nodiscard]] unsigned int GetNumber() const noexcept {
        return Index;
    }

    [[nodiscard]] T* GetData() const noexcept {
        return mPileUp;
    }

    [[nodiscard]] T GetEnd() {
        return mPileUp[Index - 1];
    }

    [[nodiscard]] void pop__() {
        if (Index == 0)return;
        --Index;
    }

    [[nodiscard]] T GetIndex(unsigned int I) {
        return mPileUp[I];
    }

    void ClearAll() {
        Index = 0;
    }

    Iterator<T> begin() {
        return Iterator<T>(mPileUp);
    }

    Iterator<T> end() {
        return Iterator<T>(mPileUp + Index);
    }
};