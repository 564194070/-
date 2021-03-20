cmake_minimum_required(VERSION 3.5)


project(chat)

#配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#设置可执行文件最后的存储路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
# 加载子目录
add_subdirectory(src)


#设置debug模式，如果没有这一行将不能调试设断点
#set(CMAKE_BUILD_TYPE "Debug")
#配置头文件搜索路径
#include_directories()
#配置库文件搜索路径
#link_directories()


#设置需要编译的源文件列表
#set(SRC_LIST ./muduoserver.cpp)

#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#把.指定路径下所有源文件名字放入变量名SRC_LIST里面
#aux_source_directory(. SRC_LIST)
#指定搜索子目录 最外层找内层的一般操作
#add_subdirectory(testmuduo)

#生成可执行文件
#add_executable(server ${SRC_LIST})
#生成库文件
#add_library()
#可执行文件(目标文件)需要连接的库
#target_link_libraries(server muduo_net muduo_base pthread)
