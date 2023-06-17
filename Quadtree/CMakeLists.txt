cmake_minimum_required(VERSION 3.5)

project(Quadtree)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	# SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/include
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
	# execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/debug/bin ${CMAKE_CURRENT_BINARY_DIR})
else()
	# execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/bin ${CMAKE_CURRENT_BINARY_DIR})
endif()

# 将源文件添加到可执行文件中
add_executable(Quadtree ${DIRSRCS})

target_link_libraries(
	Quadtree
)