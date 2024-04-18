#include <iostream>
#include <string>
#define DLLEXPORT extern "C" __declspec(dllexport)
//由于C++具有函数重载的特性 C语言的方式进行导出 避免命名冲突

DLLEXPORT void print(const char* P)//直接穿 string 会乱吗？
{
    std::cout << "[Error]: " << P << std::endl;
    std::string kao = P;
    std::cout << "[const char]: " << kao << std::endl;
    return;
}

DLLEXPORT void printStr(std::string* P)
{
    std::cout << "[string P]: " << (*P).c_str() << std::endl;
    return;
}


DLLEXPORT void ErrorLog()
{
    std::cout << "[Error]: " << std::endl;
    return;
}
