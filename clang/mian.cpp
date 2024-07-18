#include <stdio.h>
#include <clang-c/Index.h>
#include <iostream>
#include <vector>

// 将CXString转换为Std:：string
std::string CXStringToString(CXString cxString) {
    std::string result = clang_getCString(cxString);
    clang_disposeString(cxString);
    return result;
}

// 处理类成员函数名、返回类型和参数的函数
void processClassFunctions(CXCursor cursor) {
    // 确保游标是C++方法
    if (clang_getCursorKind(cursor) == CXCursor_CXXMethod) {
        // 获取函数名
        CXString functionName = clang_getCursorSpelling(cursor);
        std::string functionNameStr = CXStringToString(functionName);

        // 获取函数返回类型
        CXType returnType = clang_getCursorResultType(cursor);
        CXString returnTypeSpelling = clang_getTypeSpelling(returnType);
        std::string returnTypeStr = CXStringToString(returnTypeSpelling);

        // 获取访问规范
        CX_CXXAccessSpecifier accessSpecifier = clang_getCXXAccessSpecifier(cursor);
        std::string accessStr;
        switch (accessSpecifier) {
        case CX_CXXPublic: accessStr = "public"; break;
        case CX_CXXProtected: accessStr = "protected"; break;
        case CX_CXXPrivate: accessStr = "private"; break;
        default: accessStr = "unknown"; break;
        }

        // 输出函数签名
        std::cout << "  " << accessStr << " " << "Function: " << returnTypeStr << " " << functionNameStr << "(";

        // 获取函数参数
        std::vector<std::string> params;
        clang_visitChildren(cursor, [](CXCursor paramCursor, CXCursor parent, CXClientData client_data) {
            if (clang_getCursorKind(paramCursor) == CXCursor_ParmDecl) {
                // 获取参数类型
                CXType paramType = clang_getCursorType(paramCursor);
                CXString paramTypeSpelling = clang_getTypeSpelling(paramType);
                std::string paramTypeStr = CXStringToString(paramTypeSpelling);

                // 获取参数名称
                CXString paramName = clang_getCursorSpelling(paramCursor);
                std::string paramNameStr = CXStringToString(paramName);

                std::cout << paramTypeStr << " " << paramNameStr << " ";
            }
            return CXChildVisit_Continue;
            }, nullptr);

        std::cout << ")" << std::endl;
    }

    // 探访子集
    clang_visitChildren(cursor, [](CXCursor cursor, CXCursor parent, CXClientData client_data) {
        processClassFunctions(cursor);
        return CXChildVisit_Continue;
        }, nullptr);
}

// 使用访问规范处理类变量的函数
void processClassVariables(CXCursor cursor) {
    // 拜访班上的每个成员
    clang_visitChildren(cursor, [](CXCursor memberCursor, CXCursor parent, CXClientData client_data) {
        // 检查成员是否为变量
        if (clang_getCursorKind(memberCursor) == CXCursor_FieldDecl) {
            // 获取变量类型
            CXType varType = clang_getCursorType(memberCursor);
            CXString varTypeSpelling = clang_getTypeSpelling(varType);
            std::string varTypeStr = CXStringToString(varTypeSpelling);

            // 获取变量名称
            CXString varName = clang_getCursorSpelling(memberCursor);
            std::string varNameStr = CXStringToString(varName);

            // 获取访问规范
            CX_CXXAccessSpecifier accessSpecifier = clang_getCXXAccessSpecifier(memberCursor);
            std::string accessStr;
            switch (accessSpecifier) {
            case CX_CXXPublic: accessStr = "public"; break;
            case CX_CXXProtected: accessStr = "protected"; break;
            case CX_CXXPrivate: accessStr = "private"; break;
            default: accessStr = "unknown"; break;
            }

            // 输出变量访问规范、类型和名称
            std::cout << "  " << accessStr << " " << varTypeStr << " " << varNameStr << std::endl;
        }

        return CXChildVisit_Continue;
        }, nullptr);

    // 探访子集
    clang_visitChildren(cursor, [](CXCursor cursor, CXCursor parent, CXClientData client_data) {
        processClassVariables(cursor);
        return CXChildVisit_Continue;
        }, nullptr);
}






int main(int argc, char** argv) {
    CXIndex index = clang_createIndex(0 /* excludeDeclarationsFromPCH */,
        0 /* loadPCH */);

    // 创建一个编译单元
    CXTranslationUnit unit = clang_parseTranslationUnit(index,
        "Class.hpp", // 源代码文件名
        NULL,       // 编译命令行参数
        0,          // 参数个数
        NULL,       // 未解析宏
        0,          // 未解析宏个数
        CXTranslationUnit_None);

    if (unit == NULL) {
        fprintf(stderr, "Error parsing translation unit\n");
        return 1;
    }

    // 获取翻译单元中的顶级声明
    CXCursor cursor = clang_getTranslationUnitCursor(unit);

    // 使用访问规范处理类变量的函数
    clang_visitChildren(cursor, [](CXCursor cursor, CXCursor parent, CXClientData client_data) {
        // 是否为 Class
        if (clang_getCursorKind(cursor) == CXCursor_ClassDecl) {
            // 获取类名
            CXString className = clang_getCursorSpelling(cursor);
            std::string classNameStr = CXStringToString(className);
            std::cout << "Class: " << classNameStr << std::endl;

            processClassVariables(cursor); // 获取类的变量
            processClassFunctions(cursor); // 获取类的函数
        }
        return CXChildVisit_Continue;
        }, nullptr);


    // 清理
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);

    return 0;
}