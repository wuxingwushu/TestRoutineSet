#include "AngelScriptCode.h"



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


AngelScriptCode::AngelScriptCode() {
    // 创建AngelScript引擎
    engine = asCreateScriptEngine();
    //AngelScript引擎添加 string 类拓展
    RegisterStdString(engine);
    RegisterScriptArray(engine, false);
    // 设置引擎属性
    engine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, true);          // 允许不安全的引用
    /* 开启asEP_ALLOW_UNSAFE_REFERENCES后，可以像普通C++语法一样注册使用了 */
    engine->SetEngineProperty(asEP_OPTIMIZE_BYTECODE, true);                // 优化字节码
    engine->SetEngineProperty(asEP_INCLUDE_JIT_INSTRUCTIONS, true);         // JIT预编译字节码
    engine->SetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, true);         // 
    // 消息回调 脚本执行错误之类的
    int r = engine->SetMessageCallback(asFUNCTION(AngelScriptMessage), 0, asCALL_CDECL);
    if (r < 0) {
        std::cout << "Failed to set message callback" << std::endl;
        return;
    }
}

std::string AngelScriptCode::OpenCode(const char* FileName) {
    // 读取脚本文件
    std::ifstream file(FileName);
    if (!file.is_open()) {
        std::cout << "Failed to open script file" << std::endl;
        return {};
    }
    // 将脚本内容读取到字符串中
    std::string scriptCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return scriptCode;
}

AngelScriptBuilder AngelScriptCode::GetModuleCode(const char* Code) {
    asIScriptModule* module = engine->GetModule("MyModule");
    module->AddScriptSection("MyModule", Code);
    module->Build();
    return AngelScriptBuilder(module);
}

AngelScriptBuilder AngelScriptCode::newBuilderCode(const char* Code) {
    // 创建 ScriptBuilder
    CScriptBuilder* builder = new CScriptBuilder();
    // 绑定 ScriptBuilder 到引擎
    builder->StartNewModule(engine, "MyBuilder");
    // 导入代码
    builder->AddSectionFromMemory("MyBuilder", Code);
    // 编译脚本
    builder->BuildModule();
    return AngelScriptBuilder(builder);
}

void AngelScriptCode::RunFunction(asIScriptFunction* Function) {
    int r;
    // 准备执行脚本
    r = context->Prepare(Function);assert(r >= 0);
    // 执行脚本
    r = context->Execute(); assert(r == asEXECUTION_FINISHED);
}


AngelScriptCode::~AngelScriptCode() {
    // 释放资源
    context->Release();
    engine->ShutDownAndRelease();
}