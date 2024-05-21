cmake_minimum_required(VERSION 3.5)

project(2048)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	# SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VUH/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./
	)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	# execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}saxpy.spv ${CMAKE_CURRENT_BINARY_DIR})# 拷贝配置信息
else()
	# execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}saxpy.spv ${CMAKE_CURRENT_BINARY_DIR})# 拷贝配置信息
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./mods.ann ${CMAKE_CURRENT_BINARY_DIR})# 字体


# 将源文件添加到可执行文件中
add_executable(2048 ${DIRSRCS})

target_link_libraries(
	2048
	EasyXa.lib
)