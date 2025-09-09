# 测试专用配置
message(STATUS "Configuring tests for ${PROJECT_NAME}")

# 查找GoogleTest
find_package(GTest REQUIRED)

# 添加测试可执行文件
add_executable(${PROJECT_NAME}_tests
    Lrutest.cpp
)

# 链接依赖
target_link_libraries(${PROJECT_NAME}_tests
    PRIVATE
        zyhcache
        GTest::GTest
        GTest::Main
)

# 包含GoogleTest模块
include(GoogleTest)

# 自动发现所有测试用例
gtest_discover_tests(${PROJECT_NAME}_tests)