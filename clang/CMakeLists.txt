cmake_minimum_required(VERSION 3.5)

project(clang)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/clang18.1.8/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/clang18.1.8/lib
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/clang18.1.8/lib
	)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/clang18.1.8/bin/libclang.dll ${CMAKE_CURRENT_BINARY_DIR})
else()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/clang18.1.8/bin/libclang.dll ${CMAKE_CURRENT_BINARY_DIR})
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./Class.hpp ${CMAKE_CURRENT_BINARY_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./ ${CMAKE_CURRENT_BINARY_DIR})
# execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./NEWDLL.bat ${CMAKE_CURRENT_BINARY_DIR})

# 将源文件添加到可执行文件中
add_executable(clang ${DIRSRCS})


if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	target_link_libraries(
		clang

		libclang.lib
	)
else()
	target_link_libraries(
		clang

		libclang.lib
	)
endif()