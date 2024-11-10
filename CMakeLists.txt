# 设置最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(LayoutAlgorithmC VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# 包含 Homebrew 安装路径
include_directories(/opt/homebrew/include)  
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)  
link_directories(/opt/homebrew/lib)       

# 查找所有源文件
file(GLOB_RECURSE SOURCES "src/*.cc")


# 生成可执行文件
add_executable(MyExecutable ${SOURCES})