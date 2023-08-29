#include <iostream>
#include <functional>

class MyClass {
public:
    void memberFunction(int arg) {
        std::cout << "Member function called with argument " << arg << std::endl;
    }
};

void callback(std::function<void(int)> func, int arg) {
    func(arg);
}

int main() {
    MyClass obj;

    // 使用 bind 函数将指针和类实例组合成一个 functor
    auto func = std::bind(&MyClass::memberFunction, &obj, std::placeholders::_1);
    func(123);
    // 回调 functor
    callback(func, 123);

    return 0;
}
