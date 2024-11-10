# 设置最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(LayoutAlgorithmC VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")
# 包含 Homebrew 安装路径
include_directories(D:/workspace/layered-c/include)  

# 查找所有源文件
file(GLOB_RECURSE SOURCES "src/*.cc" "src/*.cpp" "src/*.hpp")


# 生成可执行文件
add_executable(MyExecutable ${SOURCES})
target_include_directories(MyExecutable PRIVATE D:/workspace/layered-c/include)

# # 设置构建目录为build
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# 使用Ninja作为生成器
set(CMAKE_GENERATOR "Ninja")