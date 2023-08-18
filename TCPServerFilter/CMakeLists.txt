cmake_minimum_required(VERSION 3.5)

project(TCPServerFilter)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/zlib/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/debug/lib
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/zlib/lib/Debug
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/lib
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/zlib/lib/Release
	)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/debug/bin ${CMAKE_CURRENT_BINARY_DIR})
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/zlib/bin/Debug ${CMAKE_CURRENT_BINARY_DIR})
else()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/libevent/bin ${CMAKE_CURRENT_BINARY_DIR})
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/zlib/bin/Release ${CMAKE_CURRENT_BINARY_DIR})
endif()

# 将源文件添加到可执行文件中
add_executable(TCPServerFilter ${DIRSRCS})
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	target_link_libraries(
		TCPServerFilter 
		event.lib
		event_core.lib
		event_extra.lib
		zlibd.lib
		ws2_32.lib
	)
else()
	target_link_libraries(
		TCPServerFilter 
		event.lib
		event_core.lib
		event_extra.lib
		zlib.lib
		ws2_32.lib
	)
endif()