cmake_minimum_required(VERSION 3.5)

project(AngelScript)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/AngelScript_2.36.1/angelscript/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/AngelScript_2.36.1/angelscript/lib/Debug
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/AngelScript_2.36.1/angelscript/lib/Release
	)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/AngelScript_2.36.1/angelscript/bin/Debug ${CMAKE_CURRENT_BINARY_DIR})
else()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/AngelScript_2.36.1/angelscript/bin/Release ${CMAKE_CURRENT_BINARY_DIR})
endif()

# 将源文件添加到可执行文件中
add_executable(AngelScript ${DIRSRCS})
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	target_link_libraries(
		AngelScript
		angelscript64d.lib
	)
else()
	target_link_libraries(
		AngelScript
		angelscript64.lib
	)
endif()