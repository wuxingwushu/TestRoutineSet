#include <iostream>
#include <angelscript.h>
#include "scriptstdstring.h"

void AngelScriptMessage(asSMessageInfo* msg, void* param) {
    if (msg->type == asMSGTYPE_ERROR) {
        std::cout << "Error:[" << msg->message << "]" << "Row:[" << msg->row << "]" << std::endl;
    }

    if (msg->type == asMSGTYPE_INFORMATION) {
        std::cout << "Info:[" << msg->message << "]" << "Row:[" << msg->row << "]" << std::endl;
    }

    if (msg->type == asMSGTYPE_WARNING) {
        std::cout << "Warning:[" << msg->message << "]" << "Row:[" << msg->row << "]" << std::endl;
    }
}

// 示例函数，用于注册给脚本调用
void MyFunction(int kao)
{
    std::cout << "Hello from the script!  " << kao << std::endl;
}

static int wo = 89;

int Getxx() {
    return wo;
}


int main()
{
    // 创建AngelScript引擎
    asIScriptEngine* engine = asCreateScriptEngine();

    RegisterStdString(engine);//添加字符串类型

    // 消息回调 脚本执行错误之类的
    int r = engine->SetMessageCallback(asFUNCTION(AngelScriptMessage), 0, asCALL_CDECL);
    if (r < 0) {
        std::cout << "Failed to set message callback" << std::endl;
        return -1;
    }

    // 注册要在脚本中调用的函数
    r = engine->RegisterGlobalFunction("void MyFunction(int)", asFUNCTION(MyFunction), asCALL_CDECL);
    if (r < 0) {
        std::cout << "Failed to register global function: MyFunction" << std::endl;
        engine->ShutDownAndRelease();
        return -1;
    }

    engine->RegisterGlobalProperty("int wo", &wo);//注册变量
    std::string wod;
    engine->RegisterGlobalProperty("string wod", &wod);//注册变量

    r = engine->RegisterGlobalFunction("int Getxx()", asFUNCTION(Getxx), asCALL_CDECL);
    if (r < 0) {
        std::cout << "Failed to register global function: Getxx" << std::endl;
        engine->ShutDownAndRelease();
        return -1;
    }

    // 创建一个新的脚本上下文
    asIScriptContext* context = engine->CreateContext();

    // 编译并执行脚本代码
    const char* scriptCode = R"(
        void main()
        {
            string www = "wowow d";
            MyFunction(5);
            int koa = Getxx();
            MyFunction(koa);
            MyFunction(wo);
            wo = 6;
            MyFunction(wo);
        }
    )";

    // 编译脚本
    asIScriptModule* module = engine->GetModule(0, asGM_ALWAYS_CREATE);
    module->AddScriptSection("script", scriptCode);
    module->Build();

    // 获取脚本函数句柄
    asIScriptFunction* function = module->GetFunctionByDecl("void main()");
    if (!function) {
        std::cout << "Failed to get script function" << std::endl;
        context->Release();
        engine->ShutDownAndRelease();
        return -1;
    }

    // 准备执行脚本
    r = context->Prepare(function);
    if (r < 0) {
        std::cout << "Failed to prepare script context" << std::endl;
        context->Release();
        engine->ShutDownAndRelease();
        return -1;
    }

    // 执行脚本
    r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        std::cout << "Failed to execute script" << std::endl;
    }

    // 释放资源
    context->Release();
    engine->ShutDownAndRelease();

    return 0;
}

