#pragma once
#include <angelscript.h>
#include "scriptbuilder.h"	//拓展 #include
#include "scriptstdstring.h"//拓展 string
#include "scriptarray.h"	//拓展 array
#include <cassert>
#include <iostream>
#include <fstream> 

class AngelScriptBuilder
{
public:
	AngelScriptBuilder(asIScriptModule* module):mModule(module) {};
	AngelScriptBuilder(CScriptBuilder* builder) :mBuilder(builder) { mModule = builder->GetModule(); };
	~AngelScriptBuilder() { if (mBuilder != nullptr) { delete mBuilder; } };

	asIScriptFunction* GetFunction(std::string str) {
		asIScriptFunction* Function = mModule->GetFunctionByDecl(str.c_str()); assert(Function);
		return Function;
	}
private:
	asIScriptModule* mModule = nullptr;
	CScriptBuilder* mBuilder = nullptr;
};


class AngelScriptCode
{
public:
	AngelScriptCode();
	~AngelScriptCode();

	void InitCreateContext() {
		// 创建一个新的脚本上下文
		context = engine->CreateContext();
	}

	void RunFunction(asIScriptFunction* Function);

	std::string OpenCode(const char* FileName);

	AngelScriptBuilder GetModuleCode(const char* Code);
	AngelScriptBuilder newBuilderCode(const char* Code);
	
	// AngelScript引擎
	asIScriptEngine* engine = nullptr;
	// 创建一个新的脚本上下文
	asIScriptContext* context = nullptr;
};