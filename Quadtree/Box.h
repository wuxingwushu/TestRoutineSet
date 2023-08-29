#pragma once

#include "Vector2.h"

namespace quadtree
{

template<typename T>
class Box
{
public:
    T left;
    T top;
    T width; // Must be positive
    T height; // Must be positive

    constexpr Box(T Left = 0, T Top = 0, T Width = 0, T Height = 0) noexcept :
        left(Left), top(Top), width(Width), height(Height)
    {

    }

    constexpr Box(const Vector2<T>& position, const Vector2<T>& size) noexcept :
        left(position.x), top(position.y), width(size.x), height(size.y)
    {

    }

    constexpr T getRight() const noexcept
    {
        return left + width;
    }

    constexpr T getBottom() const noexcept
    {
        return top + height;
    }

    constexpr Vector2<T> getTopLeft() const noexcept
    {
        return Vector2<T>(left, top);
    }

    //获取中心点
    constexpr Vector2<T> getCenter() const noexcept
    {
        return Vector2<T>(left + width / 2, top + height / 2);
    }

    constexpr Vector2<T> getSize() const noexcept
    {
        return Vector2<T>(width, height);
    }

    //是否包含在内 包含: true
    constexpr bool contains(const Box<T>& box) const noexcept
    {
        return left <= box.left && box.getRight() <= getRight() &&
            top <= box.top && box.getBottom() <= getBottom();
    }

    //是否有相交点 有相交点: true
    constexpr bool intersects(const Box<T>& box) const noexcept
    {
        return !(left >= box.getRight() || getRight() <= box.left ||
            top >= box.getBottom() || getBottom() <= box.top);
    }

    //点在范围内
    constexpr bool Range(const Vector2<T>& pos) const noexcept
    {
        return left <= pos.x && pos.x <= getRight() &&
            top <= pos.y && pos.y <= getBottom();
    }
};

}