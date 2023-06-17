cmake_minimum_required(VERSION 3.5)

project(SoloudMIDI)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Soloud/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Soloud/lib/Debug
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Soloud/lib/Release
	)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Soloud/bin/Debug ${CMAKE_CURRENT_BINARY_DIR})
else()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Soloud/bin/Release ${CMAKE_CURRENT_BINARY_DIR})
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./TimGM6mb.sf2 ${CMAKE_CURRENT_BINARY_DIR})# 拷贝配置信息
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./florestan-subset.sf2 ${CMAKE_CURRENT_BINARY_DIR})# 拷贝配置信息
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./夜に駆ける.mid ${CMAKE_CURRENT_BINARY_DIR})# 拷贝配置信息

# 将源文件添加到可执行文件中
add_executable(SoloudMIDI ${DIRSRCS})

target_link_libraries(
	SoloudMIDI

	#音效
	soloud.lib
	SDL2main.lib
	SDL2.lib
)