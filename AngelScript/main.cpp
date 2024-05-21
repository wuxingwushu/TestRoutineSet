#include <iostream>
#include <angelscript.h>
#include "AngelScriptCode.h"
#include <string>

// 示例函数，用于注册给脚本调用
std::string MyFunction(std::string& str)
{
    std::cout << str << std::endl;
    str = str + " - ";
    return str;
}

void MyIntPointer2(int** i)
{
    std::cout << "MyIntPointer:" << *i << std::endl;
    **i += 100;
}

void MyIntPointer(int* i)
{
    std::cout << "MyIntPointer:" << *i << std::endl;
    *i += 100;
}

void MyInt(int& i)
{
    std::cout << "MyInt:" << i << std::endl;
    i += 10;
}
//&out   只可以返回值，无法读取
//&in    只可以读取值，无法返回
//&inout 只有类才支持，除非开启 asEP_ALLOW_UNSAFE_REFERENCES 特性

// 编译并执行脚本代码
const char* scriptCode = R"(
int main(int ID)
{
    array<int> arr;
    arr.resize(2);
    arr[0] = 10;
    arr[1] = 20;

    
    MyClass obj(777);
    obj.printNumber();
    obj.printHeavyLoad();
    obj.printHeavyLoad(15);
    MyInt(ID);
    MyIntPointer(ID);
    return ID;
}
)";

class MyClass {
private:
    int myNumber;

public:
    MyClass() : myNumber(666) {}
    MyClass(int num) : myNumber(num) {}

    void printNumber() {
        std::cout << "My number is: " << myNumber << std::endl;
    }

    void printHeavyLoad(){ std::cout << "HeavyLoad" << std::endl; }
    void printHeavyLoad(int num) { std::cout << "HeavyLoad：" << num << std::endl; }
};
void ConstructMyClass(int Z, MyClass* ptr) { new(ptr) MyClass(Z); printf("int\n"); }
void ConstructMyClass2(MyClass* ptr) { new(ptr) MyClass(); printf("wu\n"); }

int main()
{
    AngelScriptCode ASOpcode;
    int r;
    r = ASOpcode.engine->RegisterGlobalFunction("string MyFunction(string&in)", asFUNCTION(MyFunction), asCALL_CDECL); assert(r >= 0);
    r = ASOpcode.engine->RegisterGlobalFunction("void MyIntOut(int &out)", asFUNCTION(MyInt), asCALL_CDECL); assert(r >= 0);
    r = ASOpcode.engine->RegisterGlobalFunction("void MyIntIn(int &in)", asFUNCTION(MyInt), asCALL_CDECL); assert(r >= 0);
    r = ASOpcode.engine->RegisterGlobalFunction("void MyInt(int &inout)", asFUNCTION(MyInt), asCALL_CDECL); assert(r >= 0);
    r = ASOpcode.engine->RegisterGlobalFunction("void MyIntPointer(int &inout)", asFUNCTION(MyIntPointer), asCALL_CDECL); assert(r >= 0);
    //r = ASOpcode.engine->RegisterGlobalFunction("void MyIntPointer2(int**)", asFUNCTION(MyIntPointer2), asCALL_CDECL); assert(r >= 0);
    // 注册 MyClass 类
    r = ASOpcode.engine->RegisterObjectType("MyClass", sizeof(MyClass), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_C); assert(r >= 0);
    r = ASOpcode.engine->RegisterObjectBehaviour("MyClass", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructMyClass2), asCALL_CDECL_OBJLAST); assert(r >= 0);//生成类
    r = ASOpcode.engine->RegisterObjectBehaviour("MyClass", asBEHAVE_CONSTRUCT, "void f(int)", asFUNCTION(ConstructMyClass), asCALL_CDECL_OBJLAST); assert(r >= 0);//生成类
    r = ASOpcode.engine->RegisterObjectMethod("MyClass", "void printNumber()", asMETHOD(MyClass, printNumber), asCALL_THISCALL); assert(r >= 0);
    // 注册重载函数
    r = ASOpcode.engine->RegisterObjectMethod("MyClass", "void printHeavyLoad()", asMETHODPR(MyClass, printHeavyLoad, (), void), asCALL_THISCALL); assert(r >= 0);
    r = ASOpcode.engine->RegisterObjectMethod("MyClass", "void printHeavyLoad(int)", asMETHODPR(MyClass, printHeavyLoad, (int), void), asCALL_THISCALL); assert(r >= 0);
    ASOpcode.InitCreateContext();
    AngelScriptBuilder Build = ASOpcode.newBuilderCode(scriptCode);
    asIScriptFunction* F = Build.GetFunction("int main(int)");

    // 准备执行脚本
    r = ASOpcode.context->Prepare(F); assert(r >= 0);
    // 设置函数输入参数
    int inputValue = 18;
    ASOpcode.context->SetArgDWord(0, inputValue); // 设置整数参数
    // 执行脚本
    r = ASOpcode.context->Execute(); assert(r == asEXECUTION_FINISHED);
    // 获取函数返回值
    std::cout << "main:" << *reinterpret_cast<int*>(ASOpcode.context->GetAddressOfReturnValue()) << std::endl;
    return 0;
}

