cmake_minimum_required(VERSION 3.5)

project(Curl_WedDav)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/curl-7.86.0/include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Poco_x64-windows/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/curl-7.86.0/lib/Debug
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Poco_x64-windows/debug/lib
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/curl-7.86.0/lib/Release
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Poco_x64-windows/lib
	)
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/curl-7.86.0/bin/Debug ${CMAKE_CURRENT_BINARY_DIR})
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Poco_x64-windows/debug/bin ${CMAKE_CURRENT_BINARY_DIR})
else()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Poco_x64-windows/bin ${CMAKE_CURRENT_BINARY_DIR})
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/curl-7.86.0/bin/Release ${CMAKE_CURRENT_BINARY_DIR})
endif()
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}./Writer.txt ${CMAKE_CURRENT_BINARY_DIR})
# 将源文件添加到可执行文件中
add_executable(Curl_WedDav ${DIRSRCS})

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	target_link_libraries(
		Curl_WedDav
		libcurl-d_imp.lib
		PocoNetd.lib
		PocoUtild.lib
		pcre2-32d.lib
	)
else()
	target_link_libraries(
		Curl_WedDav
		libcurl_imp.lib
		PocoNet.lib
		PocoUtil.lib
		pcre2-32.lib
	)
endif()
