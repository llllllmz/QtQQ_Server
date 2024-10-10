cmake_minimum_required(VERSION 3.0)
project(QQ_Server)

# 查找 muduo_base 和 muduo_network 库
# find_package(muduo_base REQUIRED)
# find_package(muduo_network REQUIRED)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 获取当前目录所有源文件
aux_source_directory(. SRC_LIST)

# 添加 nlohmann/json 库
include_directories(${CMAKE_SOURCE_DIR}/nlohmann)

# 添加可执行文件
add_executable(QQ_Server ${SRC_LIST})

# 链接 muduo_base 和 muduo_network 库
target_link_libraries(QQ_Server muduo_base muduo_net pthread mysqlclient)



