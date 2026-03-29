#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <iomanip>
#include <random>
#include <algorithm>

#include <gtest/gtest.h>
#include "cache/Lru.h" // 包含缓存类的头文件

// 辅助函数：打印结果
void printResult(const std::string& testName,
                 int capacity,
                 const std::string& name,
                 int get_operations,
                 int hits) 
{
    std::cout << "=== " << testName << " 结果 ===" << std::endl;
    std::cout << "缓存大小: " << capacity << std::endl;

    double hitRate = get_operations > 0 
        ? 100.0 * hits / get_operations 
        : 0.0;

    std::cout << name << " - 命中率: " 
              << std::fixed << std::setprecision(2) << hitRate << "% "
              << "(" << hits << "/" << get_operations << ")" 
              << std::endl << std::endl;
}

// 热点数据访问测试
int runHotDataAccessTest(zyh_cache::CachePolicy<int, std::string>& cache,
                          const std::string& name) {
    std::cout << "\n=== 测试场景1：热点数据访问测试 (" << name << ") ===" << std::endl;

    const int CAPACITY = 20;         // 缓存容量
    const int OPERATIONS = 50000;   // 总操作次数
    const int HOT_KEYS = 20;         // 热点数据数量
    const int COLD_KEYS = 5000;      // 冷数据数量

    std::random_device rd;
    std::mt19937 gen(rd());

    int hits = 0;
    int get_operations = 0;

    // 预热缓存：插入热点数据
    for (int key = 0; key < HOT_KEYS; ++key) {
        std::string value = "value" + std::to_string(key);
        cache.put(key, value);
    }

    // 模拟访问
    for (int op = 0; op < OPERATIONS; ++op) {
        bool isPut = (gen() % 100 < 30); // 30%写操作
        int key;

        if (gen() % 100 < 70) {
            key = gen() % HOT_KEYS; // 热点数据
        } else {
            key = HOT_KEYS + (gen() % COLD_KEYS); // 冷数据
        }

        if (isPut) {
            std::string value = "value" + std::to_string(key) + "_v" + std::to_string(op % 100);
            cache.put(key, value);
        } else {
            std::string result;
            get_operations++;
            if (cache.get(key, result)) {
                hits++;
            }
        }
    }

    // 打印测试结果
    printResult("热点数据访问测试", CAPACITY, name, get_operations, hits);
    return 1;
}

TEST(HotDataAccessTest, LRU) {
    zyh_cache::LruCache<int, std::string> cache(20);
    int return_code = runHotDataAccessTest(cache, "LRU");
    EXPECT_EQ(return_code, 1);
}

TEST(ModelInfoTest, LRU) {
    zyh_cache::LruCache<int, std::string> cache(1);
    EXPECT_EQ(cache.model(), "LRU缓存模型");
    EXPECT_EQ(cache.answer("你是什么模型"), "LRU缓存模型");
    EXPECT_TRUE(cache.answer("未知问题").empty());
}
