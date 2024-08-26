#include <stdio.h>
#include <clang-c/Index.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

//AutoRegister_AngelScript

/*
// 源文件名字获取 File
CXFile F =  clang_getFile(tu, "Class.hpp");
*/

// 将CXString转换为Std:：string
std::string CXStringToString(CXString cxString) {
    std::string result = clang_getCString(cxString);
    clang_disposeString(cxString);
    return result;
}

std::string extractSubstringFromFile(const std::string& filename, int startLine, int startColumn, int endLine, int endColumn) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件 " << filename << std::endl;
        return "";
    }

    std::string line;
    int lineNumber = 0; // 从0开始计数

    while (std::getline(file, line)) {
        lineNumber++;
        if (lineNumber >= startLine && lineNumber <= endLine) { // 在指定的行范围内
            int lineLength = line.size();
            if (lineNumber == startLine && lineNumber == endLine) { // 只有一行
                if (startColumn <= endColumn && startColumn <= lineLength && endColumn <= lineLength) {
                    std::string content = line.substr(startColumn - 1, endColumn - startColumn + 1);
                    file.close();
                    return content;
                }
                else {
                    std::cout << "行 " << startLine << " 的长度不足，无法提取指定列范围的内容." << std::endl;
                    file.close();
                    return "";
                }
            }
            else if (lineNumber == startLine) { // 起始行
                if (startColumn <= lineLength) {
                    std::string content = line.substr(startColumn - 1);
                    // 继续读取直到结束行
                    while (std::getline(file, line) && lineNumber < endLine) {
                        lineNumber++;
                        lineLength = line.size();
                        if (lineNumber < endLine) {
                            content += line;
                        }
                        else {
                            if (endColumn <= lineLength) {
                                content += line.substr(0, endColumn);
                            }
                            else {
                                std::cout << "行 " << endLine << " 的长度不足，无法提取指定列范围的内容." << std::endl;
                                file.close();
                                return "";
                            }
                        }
                    }
                    file.close();
                    return content;
                }
                else {
                    std::cout << "行 " << startLine << " 的长度不足，无法从指定列开始提取内容." << std::endl;
                    file.close();
                    return "";
                }
            }
            else if (lineNumber == endLine) { // 结束行
                if (endColumn <= lineLength) {
                    std::string content = line.substr(0, endColumn);
                    file.close();
                    return content;
                }
                else {
                    std::cout << "行 " << endLine << " 的长度不足，无法提取指定列范围的内容." << std::endl;
                    file.close();
                    return "";
                }
            }
            else { // 中间行
                file.close();
                return line;
            }
        }
    }

    std::cout << "文件中没有找到第 " << startLine << " 行或第 " << endLine << " 行." << std::endl;
    file.close();
    return "";
}



// 查看 Class 是否有继承与那个类
void processClassSun(CXCursor cursor) {
    // 拜访班上的每个成员
    clang_visitChildren(cursor, [](CXCursor memberCursor, CXCursor parent, CXClientData client_data) {
        // 检查成员是否为继承的类
        if (clang_getCursorKind(memberCursor) == CXCursor_CXXBaseSpecifier) {
            CXSourceRange range = clang_getCursorExtent(memberCursor);
            CXSourceLocation start = clang_getRangeStart(range);
            CXSourceLocation end = clang_getRangeEnd(range);

            CXFile file;
            unsigned int line, column, lineend, columnend;

            // Get start position
            clang_getSpellingLocation(start, &file, &line, &column, nullptr);
            CXString fileName = clang_getFileName(file);
            const char* fileNameStr = clang_getCString(fileName);
            clang_disposeString(fileName);

            // Get end position
            clang_getSpellingLocation(end, nullptr, &lineend, &columnend, nullptr);

            std::string Value = extractSubstringFromFile(fileNameStr, line, column, lineend, columnend - 1);
            std::cout << " 子类： " << Value << std::endl;
        }
        return CXChildVisit_Continue;
        }, nullptr);

    // 探访子集
    clang_visitChildren(cursor, [](CXCursor cursor, CXCursor parent, CXClientData client_data) {
        processClassSun(cursor);
        return CXChildVisit_Continue;
        }, nullptr);
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

                //是否有初始值，还没法获取内容
                clang_visitChildren(paramCursor, [](CXCursor child, CXCursor parent, CXClientData client_data) {
                    CXSourceRange range = clang_getCursorExtent(child);
                    CXSourceLocation start = clang_getRangeStart(range);
                    CXSourceLocation end = clang_getRangeEnd(range);

                    CXFile file;
                    unsigned int line, column, lineend, columnend;

                    // 获取起始位置
                    clang_getSpellingLocation(start, &file, &line, &column, nullptr);
                    CXString fileName = clang_getFileName(file);
                    const char* fileNameStr = clang_getCString(fileName);
                    clang_disposeString(fileName);

                    // 获取结束位置
                    clang_getSpellingLocation(end, nullptr, &lineend, &columnend, nullptr);

                    //std::cout << "Range for cursor at ";
                    //std::cout << fileNameStr << ":";
                    //std::cout << line << ":" << column << " - ";
                    //std::cout << lineend << ":" << columnend << std::endl;

                    std::string Value = extractSubstringFromFile(fileNameStr, line, column, lineend, columnend - 1);
                    std::cout << " = " << Value;

                    return CXChildVisit_Continue;
                    }, reinterpret_cast<CXClientData>(client_data));
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
            std::cout << "  " << accessStr << " " << varTypeStr << " " << varNameStr;

            //是否有初始值，还没法获取内容
            clang_visitChildren(memberCursor, [](CXCursor child, CXCursor parent, CXClientData client_data) {
                CXSourceRange range = clang_getCursorExtent(child);
                CXSourceLocation start = clang_getRangeStart(range);
                CXSourceLocation end = clang_getRangeEnd(range);

                CXFile file;
                unsigned int line, column, lineend, columnend;

                // Get start position
                clang_getSpellingLocation(start, &file, &line, &column, nullptr);
                CXString fileName = clang_getFileName(file);
                const char* fileNameStr = clang_getCString(fileName);
                clang_disposeString(fileName);

                // Get end position
                clang_getSpellingLocation(end, nullptr, &lineend, &columnend, nullptr);

                //std::cout << "Range for cursor at ";
                //std::cout << fileNameStr << ":";
                //std::cout << line << ":" << column << " - ";
                //std::cout << lineend << ":" << columnend << std::endl;

                std::string Value = extractSubstringFromFile(fileNameStr, line, column, lineend, columnend - 1);
                std::cout << " = " << Value;

                return CXChildVisit_Continue;
                }, reinterpret_cast<CXClientData>(client_data));
            std::cout << std::endl;
        }

        return CXChildVisit_Continue;
        }, nullptr);

    // 探访子集
    clang_visitChildren(cursor, [](CXCursor cursor, CXCursor parent, CXClientData client_data) {
        processClassVariables(cursor);
        return CXChildVisit_Continue;
        }, nullptr);
}


void processStructMembers(CXCursor cursor, CXTranslationUnit tu) {
    if (clang_getCursorKind(cursor) == CXCursor_FieldDecl) {
        CXString fieldName = clang_getCursorSpelling(cursor);
        std::string fieldNameStr = CXStringToString(fieldName);

        CXType fieldType = clang_getCursorType(cursor);
        CXString fieldTypeSpelling = clang_getTypeSpelling(fieldType);
        std::string fieldTypeStr = CXStringToString(fieldTypeSpelling);

        std::cout << "  " << fieldTypeStr << " " << fieldNameStr;

        clang_visitChildren(cursor, [](CXCursor child, CXCursor parent, CXClientData client_data) {
            CXSourceRange range = clang_getCursorExtent(child);
            CXSourceLocation start = clang_getRangeStart(range);
            CXSourceLocation end = clang_getRangeEnd(range);

            CXFile file;
            unsigned int line, column, lineend, columnend;

            // Get start position
            clang_getSpellingLocation(start, &file, &line, &column, nullptr);
            CXString fileName = clang_getFileName(file);
            const char* fileNameStr = clang_getCString(fileName);
            clang_disposeString(fileName);

            // Get end position
            clang_getSpellingLocation(end, nullptr, &lineend, &columnend, nullptr);

            //std::cout << "Range for cursor at ";
            //std::cout << fileNameStr << ":";
            //std::cout << line << ":" << column << " - ";
            //std::cout << lineend << ":" << columnend << std::endl;

            std::string Value = extractSubstringFromFile(fileNameStr, line, column, lineend, columnend - 1);
            std::cout << " = " << Value;

            return CXChildVisit_Continue;
            }, reinterpret_cast<CXClientData>(tu));

        std::cout << std::endl;
    }
    

    clang_visitChildren(cursor, [](CXCursor child, CXCursor parent, CXClientData client_data) {
        processStructMembers(child, reinterpret_cast<CXTranslationUnit>(client_data));
        return CXChildVisit_Continue;
        }, reinterpret_cast<CXClientData>(tu));
}








int main(int argc, char** argv) {

    const char* args[] = { "-std=c++17", "-I/usr/include" };

    CXIndex index = clang_createIndex(0 /* excludeDeclarationsFromPCH */,
        0 /* loadPCH */);

    // 创建一个编译单元
    CXTranslationUnit unit = clang_parseTranslationUnit(index,
        "Class.hpp", // 源代码文件名
        args,       // 编译命令行参数
        2,          // 参数个数
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
            // 获取class名
            CXString className = clang_getCursorSpelling(cursor);
            std::string classNameStr = CXStringToString(className);
            std::cout << "Class: " << classNameStr << std::endl;

            processClassSun(cursor);
            processClassVariables(cursor); // 获取类的变量
            processClassFunctions(cursor); // 获取类的函数
        }
        //是否为 Struct
        else if (clang_getCursorKind(cursor) == CXCursor_StructDecl) {
            // 获取Struct名
            CXString className = clang_getCursorSpelling(cursor);
            std::string classNameStr = CXStringToString(className);
            std::cout << "Struct: " << classNameStr << std::endl;

            processStructMembers(cursor, reinterpret_cast<CXTranslationUnit>(client_data));
        }
        return CXChildVisit_Continue;
        }, nullptr);


    // 清理
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);

    return 0;
}