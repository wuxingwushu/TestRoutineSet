cmake_minimum_required(VERSION 3.12)
project(Dithering)

# 查找并加载OpenMP模块
find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	message("Debug 模式")
else()
	message("Release 模式")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	# o2: 代码编译优化
	# -Ofast：在进行某些优化时，可能会牺牲一些标准兼容性。
	# Os: 优化代码大小,尽可能地缩小可执行文件的体积。
	# -s: 关闭调试信息
	# -march=native: 使用特定的 CPU 架构
endif()

# 设置 include 的引用头文件路径
include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/GLFW/include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VulKan/Include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Freetype/include
)

# 设置 lib 的文件路径（区分 Debug / Release）
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/GLFW/lib
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VulKan/Lib
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Freetype/lib/Debug
	)
else()
	link_directories(
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/GLFW/lib
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/VulKan/Lib
		SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}./../Environment/Freetype/lib/Release
	)
endif()



if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
else()
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}./../src/ImGui/Minecraft_AE.ttf ${CMAKE_CURRENT_BINARY_DIR})# 字体

# 在 FastNoiseLite 以及添加过一次了不需要在添加 直接 target_link_libraries 导入对应的Lib
# add_subdirectory(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src)
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/Vulkan ${CMAKE_BINARY_DIR}/src/Vulkan)
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/VulkanTool ${CMAKE_BINARY_DIR}/src/VulkanTool)
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/ImGui ${CMAKE_BINARY_DIR}/src/ImGui)
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/Tool ${CMAKE_BINARY_DIR}/src/Tool)

add_executable(Dithering  ${DIRSRCS})

# 预处理定义
#target_compile_definitions( VulKan PRIVATE _CONSOLE)
#target_compile_definitions(VulKan PRIVATE -DWIN32_LEAN_AND_MEAN)

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
	target_link_libraries(
		#设置启动项
		Dithering 
	
		# 文件夹代码
		vulkanLib 
		vulkanToolLib
		imguiLib
		ToolLib
		srcLib
	
		# VulKan API
		vulkan-1.lib 
		
		#跨平台窗口
		glfw3.lib
	
		# 多线程
		OpenMP::OpenMP_CXX

		# 字符 ttf
		freetyped.lib
	)
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")#关闭控制台


	target_link_libraries(
		#设置启动项
		Dithering 
	
		# 文件夹代码
		vulkanLib
		vulkanToolLib
		imguiLib
		ToolLib
		srcLib
	
		# VulKan API
		vulkan-1.lib 
		
		#跨平台窗口
		glfw3.lib

		# 多线程
		OpenMP::OpenMP_CXX

		# 字符 ttf
		freetype.lib
	)
endif()


