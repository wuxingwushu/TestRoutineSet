cmake_minimum_required(VERSION 3.5)


# TCPA/B 网络连接交换数据
add_subdirectory(TCPA build/TCPA)
add_subdirectory(TCPB build/TCPB)
# event2 建立 TCP 连接 zlib 压缩传输的数据
add_subdirectory(test_buffer_filter_zlib build/test_buffer_filter_zlib)
# TCP Client Filter(过滤器压缩数据量)
add_subdirectory(TCPClientFilter build/TCPClientFilter)
add_subdirectory(TCPServerFilter build/TCPServerFilter)
# 迷宫生成
add_subdirectory(Labyrinth build/Labyrinth)
add_subdirectory(GenerateMaze build/GenerateMaze)
# GPU计算 两个库
add_subdirectory(VUH build/VUH)
add_subdirectory(Kompute build/Kompute)
add_subdirectory(VulKan_Mandelbrot build/VulKan_Mandelbrot) # 纯VulKan GUP计算
# Brensenham 直线
add_subdirectory(Brensenham build/Brensenham)
add_subdirectory(Dithering build/Dithering)# 不同直线实现效果
# MIDI 项目
add_subdirectory(MIDI build/MIDI)# sf2音色
add_subdirectory(SoloudMIDI build/SoloudMIDI)# Soloud 支持 MIDI
# C++ 实现 CPU光栅化
add_subdirectory(C++Rasterization build/C++Rasterization)
# 四叉树
add_subdirectory(Quadtree build/Quadtree)
# 二进制搜索树
add_subdirectory(RedBlackTree build/RedBlackTree)
# 耗时检测库
add_subdirectory(Timer build/Timer)
# 连续区块移动
add_subdirectory(ContinuousPlate build/ContinuousPlate)
add_subdirectory(MovePlate build/MovePlate)# 支持区块查询
# AngelScript 天使脚本
add_subdirectory(AngelScript build/AngelScript)
# DLL 函数 读取 调用
add_subdirectory(DLL build/DLL)
# 坚果云 WedDav
add_subdirectory(Curl_WedDav build/Curl_WedDav)
# 导入ReptilesYouDao项目
add_subdirectory(ReptilesYouDao build/ReptilesYouDao)
# A_Star 寻路算法
add_subdirectory(A_Star_Algorithm build/A_Star_Algorithm)
add_subdirectory(JPS build/JPS)# A_Star 改良版 JPS 寻路
add_subdirectory(jps_grid build/jps_grid)
# 多叉树实现预测指令
add_subdirectory(Trie build/Trie)
# 有限状态机
add_subdirectory(sml build/sml)
# 类 的多态
add_subdirectory(virtual build/virtual)
# 导入WFC项目
add_subdirectory(WFC build/WFC)
# 柏林噪声
add_subdirectory(FastNoiseLite build/FastNoiseLite)
# 调用矢量字体
add_subdirectory(Freetype build/Freetype)
add_subdirectory(TTF_bin build/TTF_bin)# 矢量字体 生成 位图字体
# 字符串生成二维码
add_subdirectory(QRCode build/QRCode)
# 神经网络
add_subdirectory(BP build/BP)
add_subdirectory(ANN build/ANN)
# Hough识别矩形框
add_subdirectory(Hough build/Hough)
# 机械臂控制代码
add_subdirectory(MechanicalArm build/MechanicalArm)
# 2048 Ai
add_subdirectory(2048 build/2048)
# 网格道路生成图
add_subdirectory(GridNavigation build/GridNavigation)
# 解析.h 头文件生成注册文件
add_subdirectory(clang build/clang)
# 罗汉塔解体步骤
add_subdirectory(ArhatTower build/ArhatTower)
