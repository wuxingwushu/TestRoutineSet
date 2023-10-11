cmake_minimum_required(VERSION 3.5)

project(jps_grid)

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

execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./sample_map_in_5x5.txt ${CMAKE_CURRENT_BINARY_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./sample_map_in_30x30.txt ${CMAKE_CURRENT_BINARY_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./sample_map_in_100x100.txt ${CMAKE_CURRENT_BINARY_DIR})

# 将源文件添加到可执行文件中
add_executable(jps_grid ${DIRSRCS})

target_link_libraries(
	jps_grid
)