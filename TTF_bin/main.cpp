#include <iostream>
#include <fstream>
#include <freetype/freetype.h>
#include <codecvt>
#include FT_FREETYPE_H
#include <string>
#include <stdlib.h>
#include <wchar.h>

FT_Library library;
FT_Face face;

typedef struct FontInformation {
    unsigned int Deviation;
    unsigned char x;
    unsigned char y;
    char Dx;
    char Dy;
};

typedef struct FontRange {
    unsigned short Head;
    unsigned short Tail;
    unsigned short Deviation;
};

unsigned int from_bytes_int(const char* bytes) {
    unsigned int result;

    unsigned char c = bytes[0];  // 将有符号字节转换为无符号字节
    if (c <= 0x7F) {
        result = c;  // 单字节 ASCII 字符
    }
    else if (c < 0xE0) {
        // 双字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        result = (c & 0x1F) << 6 | (c2 & 0x3F);
    }
    else if (c < 0xF0) {
        // 三字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        result = (c & 0x0F) << 12 | (c2 & 0x3F) << 6 | (c3 & 0x3F);
    }
    else if (c < 0xF8) {
        // 四字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        unsigned char c4 = bytes[3];
        result = (c & 0x07) << 18 | (c2 & 0x3F) << 12 | (c3 & 0x3F) << 6 | (c4 & 0x3F);
    }
    else if (c < 0xFC) {
        // 五字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        unsigned char c4 = bytes[3];
        unsigned char c5 = bytes[4];
        result = (c & 0x03) << 24 | (c2 & 0x3F) << 18 | (c3 & 0x3F) << 12 | (c4 & 0x3F) << 6 | (c5 & 0x3F);
    }
    else if (c < 0xFE) {
        // 六字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        unsigned char c4 = bytes[3];
        unsigned char c5 = bytes[4];
        unsigned char c6 = bytes[5];
        result = (c & 0x01) << 30 | (c2 & 0x3F) << 24 | (c3 & 0x3F) << 18 | (c4 & 0x3F) << 12 | (c5 & 0x3F) << 6 | (c6 & 0x3F);
    }
    else {
        // 不支持的字符编码
        throw std::runtime_error("Invalid UTF-8 byte sequence");
    }

    return result;
}

unsigned short from_bytes(const char* bytes) {
    unsigned short result;

    unsigned char c = bytes[0];  // 将有符号字节转换为无符号字节
    if (c <= 0x7F) {
        result = c;  // 单字节 ASCII 字符
    }
    else if (c < 0xE0) {
        // 双字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        result = (c & 0x1F) << 6 | (c2 & 0x3F);
    }
    else if (c < 0xF0) {
        // 三字节 UTF-8 字符
        unsigned char c2 = bytes[1];
        unsigned char c3 = bytes[2];
        result = (c & 0x0F) << 12 | (c2 & 0x3F) << 6 | (c3 & 0x3F);
    }
    else {
        // 不支持的字符编码
        throw std::runtime_error("Invalid UTF-8 byte sequence");
    }

    return result;
}


const char Info_1[] = R"(
typedef struct FontInformation {
    unsigned int Deviation;
    unsigned char x;
    unsigned char y;
    char Dx;
    char Dy;
};

typedef struct FontRange {
    unsigned short Head;
    unsigned short Tail;
    unsigned short Deviation;
};

const FontRange Range[] = {
)";

const char Info_2[] = R"(};

unsigned short from_Deviation(unsigned int bytes){
  for(unsigned int i = 0; i < sizeof(Range) / sizeof(FontRange); ++i){
    if((Range[i].Head <= bytes) && (bytes <=  Range[i].Tail)){
      bytes -= Range[i].Deviation;
      return bytes;
    }
  }
  return 0xFFFF;
}

unsigned char fromDeviation = 0;
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

#define From_Bin 1

#if From_Bin
const FontInformation FontInfo[] = {
)";


int main() {
    if (FT_Init_FreeType(&library)) {
        std::cerr << "Failed to initialize FreeType library" << std::endl;
        return -1;
    }

    if (FT_New_Face(library, "./Minecraft_AE.ttf", 0, &face)) {
        std::cerr << "Failed to load font" << std::endl;
        FT_Done_FreeType(library);
        return -1;
    }

    if (FT_Set_Char_Size(face, 0, 16 * 64, 72, 72)) {
        std::cerr << "Failed to set character size" << std::endl;
        FT_Done_Face(face);
        FT_Done_FreeType(library);
        return -1;
    }


    // 打开二进制文件
    std::ofstream FontInfo("FontInfo.bin", std::ios::out | std::ios::binary);
    std::ofstream FontData("FontData.bin", std::ios::out | std::ios::binary);
    std::ofstream file("FontFunction.h");
    


    

    unsigned short ADDDeviation = 0;
    //https://www.cnblogs.com/findumars/p/6833786.html
    //https://www.jrgraphix.net/r/Unicode/F900-FAFF
    FontRange Range[] = {
        { 0x0020, 0x007F, 0 },//Basic Latin
        { 0x2000, 0x206F, 0 },//General Punctuation
        { 0x2E80, 0x2FDF, 0 },//CJK Radicals Supplement ~ Kangxi Radicals
        { 0x3000, 0x30FF, 0 },//CJK Symbols and Punctuation ~ Hiragana ~ Katakana
        { 0x31F0, 0x31FF, 0 },//Katakana Phonetic Extensions
        { 0x3400, 0x4DBF, 0 },//CJK Unified Ideographs Extension A
        { 0x4E00, 0x9FFF, 0 },//CJK Unified Ideographs
        { 0xF900, 0xFAFF, 0 },//CJK Compatibility Ideographs
        { 0xFE30, 0xFE4F, 0 },//CJK Compatibility Forms
        { 0xFF00, 0xFFEF, 0 },//Halfwidth and Fullwidth Forms
    };

    file << Info_1;
    for (size_t iR = 0; iR < sizeof(Range) / sizeof(FontRange); ++iR) {
        Range[iR].Deviation = Range[iR].Head - ADDDeviation;
        ADDDeviation += (Range[iR].Tail + 1) - Range[iR].Head;
        file << "\t{ " << std::to_string(Range[iR].Head) << ", "
            << std::to_string(Range[iR].Tail) << ", "
            << std::to_string(Range[iR].Deviation) << " },\n";
        std::cout << "Head ：" << Range[iR].Head << "   Tail ：" << Range[iR].Tail << "   Deviation ：" << Range[iR].Deviation << std::endl;
    }
    file << Info_2;

    if (!FontData) {
        std::cout << "无法打开文件！" << std::endl;
        return 1;
    }

    FT_UInt glyph_index;
    FT_BBox bbox;
    char CharBuffer[4096];
    unsigned int Index = 0;
    unsigned int dddd = 0;
    FontInformation Info;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    

    for (size_t iR = 0; iR < sizeof(Range) / sizeof(FontRange); ++iR)
    {
        for (unsigned short i = Range[iR].Head; i <= Range[iR].Tail; ++i) {
            //std::cout << converter.to_bytes(i) << std::endl;
            glyph_index = FT_Get_Char_Index(face, i);
            if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
                continue;
            }
            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            Info.Deviation = Index;
            Info.x = face->glyph->bitmap.width;
            Info.y = face->glyph->bitmap.rows;
            Info.Dx = face->glyph->bitmap_left;
            Info.Dy = 15 - face->glyph->bitmap_top;
            for (size_t i = 0; i < sizeof(FontInformation); i++)
            {
                FontInfo.write((((char*)&Info) + i), 1);
            }

            unsigned int BufferSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
            file << "\t{ " << std::to_string(Index) << ", "
                << std::to_string(face->glyph->bitmap.width) << ", "
                << std::to_string(face->glyph->bitmap.rows) << ", "
                << std::to_string(face->glyph->bitmap_left) << ", "
                << std::to_string(15 - face->glyph->bitmap_top) << " },"
                << "//" << converter.to_bytes(i) << ", " << i << ", " << face->glyph->bitmap_left << " - " << 15 - face->glyph->bitmap_top << "\n";

            unsigned int BufferIndex = 0;
            char* CharBuffer_P = CharBuffer;
            unsigned char Char_B = 0;
            dddd = 0;
            for (size_t Xsize = 0; Xsize < face->glyph->bitmap.width; ++Xsize)
            {
                for (size_t Ysize = 0; Ysize < face->glyph->bitmap.rows; ++Ysize)
                {
                    ++BufferIndex;
                    Char_B = Char_B << 1;
                    if (face->glyph->bitmap.buffer[(Xsize * face->glyph->bitmap.rows) + Ysize]) {
                        Char_B += 0x01;
                        ++dddd;
                    }
                    if (BufferIndex == 8) {
                        BufferIndex = 0;
                        *CharBuffer_P = Char_B;
                        Char_B = 0;
                        ++CharBuffer_P;
                    }
                }
            }
            if (BufferIndex != 0) {
                Char_B = Char_B << (8 - BufferIndex);
                *CharBuffer_P = Char_B;
            }
            BufferSize = (BufferSize / 8) + ((BufferSize % 8) != 0 ? 1 : 0);
            FontData.write(CharBuffer, BufferSize);
            Index += BufferSize;
            //face->glyph->bitmap = { 0 };
        }
    }
    

    file << "};\n#endif\n";

    // 关闭文件
    FontInfo.close();
    FontData.close();
    file.close();

    std::cout << "数据已经成功写入文件！" << std::endl; 
    std::cout << from_bytes("—") << " - " << from_bytes_int("—") << std::endl;
    return 0;
}
