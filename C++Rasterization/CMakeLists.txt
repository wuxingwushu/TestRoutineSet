cmake_minimum_required(VERSION 3.5)

project(C++Rasterization)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

aux_source_directory(. DIRSRCS)

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VulKan/Include
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
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./../Environment/VulKan/Bin/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR})
else()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./../Environment/VulKan/Bin/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR})
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./image ${CMAKE_CURRENT_BINARY_DIR}./image)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./objs ${CMAKE_CURRENT_BINARY_DIR}./objs)


# 将源文件添加到可执行文件中
add_executable(C++Rasterization ${DIRSRCS})

# 预处理定义
# target_compile_definitions(C++Rasterization PRIVATE _WINDOWS)

target_link_libraries(
	C++Rasterization

	SDL2.lib
)