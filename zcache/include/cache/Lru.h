#pragma once 

#include <cstring>
#include <list>
#include <memory>
#include <vector>
#include <thread>
#include <cmath>
#include <mutex>
#include <unordered_map>

#include "base/base.h"

namespace zyh_cache
{

// 前向声明，告诉编译器存在一个名为LruCache的类模板，它接受两个类型参数Key和Value
// 但没有提供该类的具体定义。这通常用于解决循环依赖或提前声明的情况。
template<typename Key, typename Value> class LruCache;

template<typename Key, typename Value>
class LruNode 
{
private:
    Key key_;
    Value value_;
    size_t accessCount_;  // 访问次数
    std::weak_ptr<LruNode<Key, Value>> prev_;  // weak_ptr,不增加引用计数，打破循环引用
    std::shared_ptr<LruNode<Key, Value>> next_;

public:
    LruNode(Key key, Value value)
        : key_(key)
        , value_(value)
        , accessCount_(1) 
    {}

    // 提供必要的访问器
    Key getKey() const { return key_; }
    Value getValue() const { return value_; }
    void setValue(const Value& value) { value_ = value; }
    size_t getAccessCount() const { return accessCount_; }
    void incrementAccessCount() { ++accessCount_; }

    friend class LruCache<Key, Value>;
};


template<typename Key, typename Value>
class LruCache : public CachePolicy<Key, Value>
{
public:
    using LruNodeType = LruNode<Key, Value>;
    using NodePtr = std::shared_ptr<LruNodeType>;
    using NodeMap = std::unordered_map<Key, NodePtr>;

    LruCache(int capacity);
    ~LruCache() override = default;

    // 添加缓存
    void put(Key key, Value value) override;
    
    bool get(Key key, Value& value) override;

    Value get(Key key) override;

    std::string model() const override;

    // 删除指定元素
    void remove(Key key);

private:
    void initializeList();

    void updateExistingNode(NodePtr node, const Value& value);

    void addNewNode(const Key& key, const Value& value); 

    // 将该节点移动到最新的位置
    void moveToMostRecent(NodePtr node);

    void removeNode(NodePtr node);

    // 从尾部插入结点
    void insertNode(NodePtr node);

    // 驱逐最近最少访问
    void evictLeastRecent();

private:
    int           capacity_; // 缓存容量
    NodeMap       nodeMap_; // key -> Node 
    std::mutex    mutex_;
    NodePtr       dummyHead_; // 虚拟头结点
    NodePtr       dummyTail_;
};

} // namespace KamaCache

#include "cache/Lru.tpp"
