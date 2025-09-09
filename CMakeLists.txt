# 设置最低 CMake 版本要求
cmake_minimum_required(VERSION 3.10)
# 启用生成 compile_commands.json 文件，该文件会记录项目中所有源文件的编译命令（包括编译器选项、头文件路径等元数据）。
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置项目名称
project(zcache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# 添加模块库目标
aux_source_directory(${PROJECT_NAME}/src DIR_zcache_SRC)

# 指定库文件输出到项目根目录下的 lib 文件夹中
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# 收集cpp文件，但这个项目没有

# 创建静态库
# 不编译任何 cpp，只提供头文件
add_library(zyhcache INTERFACE)  # INTERFACE 库
target_include_directories(zyhcache
    INTERFACE ${PROJECT_SOURCE_DIR}/zcache/include
)

# 有条件地包含测试
option(BUILD_TESTING "Build the tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)  # 关键点：引用测试子目录
endif()