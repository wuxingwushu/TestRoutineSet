#include <iostream>
#include <Windows.h>

// 声明DLL函数原型
typedef void(*AddNumbersFunc)(const char*);
typedef void(*AddNumbersFuncP)(std::string*);
typedef void(*AddNumbersFuncE)();



int main() {
    system("NEWDLL.bat");

    // 加载DLL
    HINSTANCE hDll = LoadLibrary("D.dll");
    if (hDll == nullptr) {
        std::cout << "无法加载DLL文件" << std::endl;
        return 1;
    }

    // 获取DLL函数地址
    AddNumbersFunc addNumbers = (AddNumbersFunc)GetProcAddress(hDll, "print");
    if (addNumbers == nullptr) {
        std::cout << "无法找到DLL函数" << std::endl;
        return 1;
    }
    AddNumbersFuncP addNumbersP = (AddNumbersFuncP)GetProcAddress(hDll, "printStr");
    if (addNumbers == nullptr) {
        std::cout << "无法找到DLL函数" << std::endl;
        return 1;
    }
    AddNumbersFuncE addNumbersE = (AddNumbersFuncE)GetProcAddress(hDll, "ErrorLog");
    if (addNumbers == nullptr) {
        std::cout << "无法找到DLL函数" << std::endl;
        return 1;
    }

    std::string Str = "DLL OK!";
    std::string* StrP = new std::string;
    *StrP = Str;

    // 调用DLL函数
    addNumbersP(StrP);
    addNumbers(Str.c_str());
    addNumbersE();

    // 卸载DLL
    FreeLibrary(hDll);

	return 0;
}