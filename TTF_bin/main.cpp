#include <iostream>
#include <fstream>
#include <freetype/freetype.h>
#include <codecvt>
#include FT_FREETYPE_H
#include <string>
#include <stdlib.h>
#include <wchar.h>

const char Info_1[] = R"(
// 字体位图信息
typedef struct FontInformation
{
    unsigned int Deviation; // 字体在位图数据中的偏移量
    unsigned char x;        // 字体位图宽
    unsigned char y;        // 字体位图高
    char Dx;                // 字体显示位置左右偏移
    char Dy;                // 字体显示位置高度偏移
};

// UTF-8 范围对应的字体位图信息索引表的偏移值
typedef struct FontRange
{
    unsigned short Head;      // UTF-8 的 起始范围
    unsigned short Tail;      // UTF-8 的 结束范围
    unsigned short Deviation; // 字体位图信息索引表的偏移值
};

const FontRange UTF8Range[] = {
)";

const char Info_2[] = R"(};

/**
 * @brief UTF-8 转 对应的字体位图索引值
 * @param bytes UTF8字 的 整型值
 * @return 字符串第一个UTF8字的位图索引值 */
unsigned short from_Deviation(unsigned int bytes){
  for(unsigned int i = 0; i < sizeof(UTF8Range) / sizeof(FontRange); ++i){
    if((UTF8Range[i].Head <= bytes) && (bytes <=  UTF8Range[i].Tail)){
      bytes -= UTF8Range[i].Deviation;
      return bytes;
    }
  }
  return 0xFFFF;
}

// 存储当前检测的 UTF8 占多少字节
unsigned char fromDeviation = 0;
/**
 * @brief UTF-8 转 对应的字体位图索引值
 * @param bytes UTF8字符串
 * @return 字符串第一个UTF8字的位图索引值 */
unsigned short from_bytes(const char* bytes) {
    unsigned int result;

    unsigned char c = bytes[0];  // 将有符号字节转换为无符号字节
    if (c <= 0x7F) {
        fromDeviation = 1;
        result = c;  // 单字节 ASCII 字符
    }
    else if (c < 0xE0) {
        // 双字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        fromDeviation = 2;
        result = (c & 0x1F) << 6 | (c2 & 0x3F);
    }
    else if (c < 0xF0) {
        // 三字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        fromDeviation = 3;
        result = (c & 0x0F) << 12 | (c2 & 0x3F) << 6 | (c3 & 0x3F);
    }
    else if (c < 0xF8) {
        // 四字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        unsigned char c4 = bytes[3];
        fromDeviation = 4;
        result = (c & 0x07) << 18 | (c2 & 0x3F) << 12 | (c3 & 0x3F) << 6 | (c4 & 0x3F);
    }
    else if (c < 0xFC) {
        // 五字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        unsigned char c4 = bytes[3];
        unsigned char c5 = bytes[4];
        fromDeviation = 5;
        result = (c & 0x03) << 24 | (c2 & 0x3F) << 18 | (c3 & 0x3F) << 12 | (c4 & 0x3F) << 6 | (c5 & 0x3F);
    }
    else if (c < 0xFE) {
        // 六字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        unsigned char c4 = bytes[3];
        unsigned char c5 = bytes[4];
        unsigned char c6 = bytes[5];
        fromDeviation = 6;
        result = (c & 0x01) << 30 | (c2 & 0x3F) << 24 | (c3 & 0x3F) << 18 | (c4 & 0x3F) << 12 | (c5 & 0x3F) << 6 | (c6 & 0x3F);
    }
    else {
        // 不支持的字符编码
        //throw std::runtime_error("Invalid UTF-8 byte sequence");
        fromDeviation = 6;
        return 0xFFFF;
    }

    return from_Deviation(result);
}

// 是否以 .h 的形式存储字体位图信息
#define FromInfo_Bin 0

#if FromInfo_Bin
// 字体位图信息
const FontInformation FontInfo[] = {
)";

// 字体位图信息
typedef struct FontInformation
{
    unsigned int Deviation; // 字体在位图数据中的偏移量
    unsigned char x;        // 字体位图宽
    unsigned char y;        // 字体位图高
    char Dx;                // 字体显示位置左右偏移
    char Dy;                // 字体显示位置高度偏移
};

// UTF-8 字片段范围 对应的 字体位图信息索引表的偏移值
typedef struct FontRange
{
    unsigned short Head;      // UTF-8 的 起始范围
    unsigned short Tail;      // UTF-8 的 结束范围
    unsigned short Deviation; // 字体位图信息索引表的偏移值
};

int main()
{
    FT_Library library;
    FT_Face face;

    // 初始化 FreeType
    if (FT_Init_FreeType(&library))
    {
        std::cerr << "Failed to initialize FreeType library" << std::endl;
        return -1;
    }

    // 读取矢量字体文件
    if (FT_New_Face(library, "./Minecraft_AE.ttf", 0, &face))
    {
        std::cerr << "Failed to load font" << std::endl;
        FT_Done_FreeType(library);
        return -1;
    }

    // 设置字体大小
    if (FT_Set_Char_Size(face, 0, 16 * 64, 72, 72))
    {
        std::cerr << "Failed to set character size" << std::endl;
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return -1;
    }

    // 打开二进制文件
    std::ofstream FontInfo("FontInfo.bin", std::ios::out | std::ios::binary); // 字体位图索引表 bit文件
    std::ofstream FontData("FontData.bin", std::ios::out | std::ios::binary); // 字体位图 bit文件
    std::ofstream file("FontFunction.h");                                     // 字体位图读取函数

    // 文件是否都打开了
    if (!FontData || !FontInfo || !file)
    {
        std::cout << "无法打开文件！" << std::endl;
        return -1;
    }

    // https://www.cnblogs.com/findumars/p/6833786.html
    // https://www.jrgraphix.net/r/Unicode/F900-FAFF
    // 添加你需要的UTF8字体范围
    FontRange UTF8Range[] = {
        {0x0020, 0x007F, 0}, // Basic Latin
        {0x2000, 0x206F, 0}, // General Punctuation
        {0x2E80, 0x2FDF, 0}, // CJK Radicals Supplement ~ Kangxi Radicals
        {0x3000, 0x30FF, 0}, // CJK Symbols and Punctuation ~ Hiragana ~ Katakana
        {0x31F0, 0x31FF, 0}, // Katakana Phonetic Extensions
        {0x3400, 0x4DBF, 0}, // CJK Unified Ideographs Extension A
        {0x4E00, 0x9FFF, 0}, // CJK Unified Ideographs
        {0xF900, 0xFAFF, 0}, // CJK Compatibility Ideographs
        {0xFE30, 0xFE4F, 0}, // CJK Compatibility Forms
        {0xFF00, 0xFFEF, 0}, // Halfwidth and Fullwidth Forms
    };

    // 向 .h 写入固定信息
    file << Info_1;
    // 字体 UTF8 的范围偏移值
    unsigned short ADDDeviation = 0;
    for (size_t iR = 0; iR < sizeof(UTF8Range) / sizeof(FontRange); ++iR)
    {
        // 获取当中字范围的偏移值
        UTF8Range[iR].Deviation = UTF8Range[iR].Head - ADDDeviation;
        // 偏移值 累加 这个范围内的字数
        ADDDeviation += (UTF8Range[iR].Tail - UTF8Range[iR].Head) + 1;
        // 将数据 写入 .h
        file << "\t{ " << std::to_string(UTF8Range[iR].Head) << ", "
             << std::to_string(UTF8Range[iR].Tail) << ", "
             << std::to_string(UTF8Range[iR].Deviation) << " },\n";
        // 打印信息
        std::cout << "Head ：" << UTF8Range[iR].Head << "   Tail ：" << UTF8Range[iR].Tail << "   Deviation ：" << UTF8Range[iR].Deviation << std::endl;
    }
    // 向 .h 写入固定信息
    file << Info_2;

    char CharBuffer[4096];  // 临时存储位图数据
    unsigned int Index = 0; // 存储当前位图数据已使用字节总量
    FontInformation Info;   // 存储字信息
    // 将 utf-8 的整型转为 字（字符串） 的 工具
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    for (size_t iR = 0; iR < sizeof(UTF8Range) / sizeof(FontRange); ++iR)
    {
        for (unsigned short i = UTF8Range[iR].Head; i <= UTF8Range[iR].Tail; ++i)
        {
            // 打印打前正在处理的字
            std::cout << converter.to_bytes(i);
            // 获取对应字的索引
            FT_UInt glyph_index = FT_Get_Char_Index(face, i);
            // 是否存在这个字的矢量信息
            if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT))
            {
                // 不存这个字的矢量信息，数据链不完整
                std::cout << "存在缺失字的矢量信息！" << std::endl;
                return -1;
            }
            // 获取字的信息
            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            Info.Deviation = Index;                                   // 获取字位图偏移量
            Info.x = face->glyph->bitmap.width;                       // 获取位图宽
            Info.y = face->glyph->bitmap.rows;                        // 获取位图高
            Info.Dx = face->glyph->bitmap_left;                       // 获取位图位置 x 偏移
            Info.Dy = 15 - face->glyph->bitmap_top;                   // 获取位图位置 y 偏移
            FontInfo.write(((char *)&Info), sizeof(FontInformation)); // 将数据以 bit 的形写入

            // 再将字的信息以文本方式存储在 .h 文件中
            file << "\t{ " << std::to_string(Info.Deviation) << ", "
                 << std::to_string(Info.x) << ", "
                 << std::to_string(Info.y) << ", "
                 << std::to_string(Info.Dx) << ", "
                 << std::to_string(15 - Info.Dy) << " },"
                 << "//" << converter.to_bytes(i) << ", " << i << "\n";

            // 记录当前是第几 bit 数据
            unsigned int BufferIndex = 0;
            // 获取存储位图数据指针
            char *CharBuffer_P = CharBuffer;
            // 临时存储 一字节 位图数据
            unsigned char Char_B = 0;
            // 开始循环取 bit 数据
            for (size_t Xsize = 0; Xsize < Info.x; ++Xsize)
            {
                for (size_t Ysize = 0; Ysize < Info.y; ++Ysize)
                {
                    // bit 数据 +1
                    ++BufferIndex;
                    // 左移 1 bit
                    Char_B <<= 1;
                    // 判断这个像素亮灭
                    if (face->glyph->bitmap.buffer[(Xsize * face->glyph->bitmap.rows) + Ysize])
                    {
                        // 亮就 置 1
                        ++Char_B;
                    }
                    // 是否存储满了 8bit
                    if (BufferIndex == 8)
                    {
                        // bit 记位清零
                        BufferIndex = 0;
                        // 将数据存储起来
                        *CharBuffer_P = Char_B;
                        // 临时数据清零
                        Char_B = 0;
                        // 指针向后移
                        ++CharBuffer_P;
                    }
                }
            }
            // 判断是否有数据没有存储（不满 8bit ）
            if (BufferIndex != 0)
            {
                // 数据移置高位
                Char_B = Char_B << (8 - BufferIndex);
                // 存储数据
                *CharBuffer_P = Char_B;
            }
            // 总共有多少bit数据
            unsigned int BufferSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
            // 转换为占用多少字节
            BufferSize = (BufferSize / 8) + ((BufferSize % 8) != 0 ? 1 : 0);
            // 将数据都已 bit 形式写入
            FontData.write(CharBuffer, BufferSize);
            // 累计占用字节数
            Index += BufferSize;
        }
    }

    // 向 .h 写入固定结尾
    file << "};\n#endif\n\n";

    // 关闭文件
    FontInfo.close();
    FontData.close();
    file.close();
    return 0;
}
