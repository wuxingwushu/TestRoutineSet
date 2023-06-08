cmake_minimum_required(VERSION 3.5)

project(VulKan_Mandelbrot)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VulKan/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VulKan/Lib
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VulKan/Lib
	)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./comp.spv ${CMAKE_CURRENT_BINARY_DIR})# 拷贝配置信息
else()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./comp.spv ${CMAKE_CURRENT_BINARY_DIR})# 拷贝配置信息
endif()

# 将源文件添加到可执行文件中
add_executable(VulKan_Mandelbrot main.cpp lodepng.cpp)

target_link_libraries(
	VulKan_Mandelbrot
	# VulKan API
	vulkan-1.lib
)