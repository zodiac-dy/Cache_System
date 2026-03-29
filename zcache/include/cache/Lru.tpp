#include "cache/Lru.h"

namespace zyh_cache
{

// LruCache类成员函数的实现

template<typename Key, typename Value>
LruCache<Key, Value>::LruCache(int capacity): capacity_(capacity)
{
    initializeList();
}

template <typename Key, typename Value>
void LruCache<Key, Value>::put(Key key, Value value)
{   
    if (capacity_ <= 0)
        return;

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodeMap_.find(key);
    if (it != nodeMap_.end())
    {
        // 如果在当前容器中,则更新value,并调用get方法，代表该数据刚被访问
        updateExistingNode(it->second, value);
        return ;
    }

    addNewNode(key, value);
}   

template <typename Key, typename Value>
bool LruCache<Key, Value>::get(Key key, Value& value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodeMap_.find(key);
    if (it != nodeMap_.end())
    {
        moveToMostRecent(it->second);
        value = it->second->getValue();
        return true;
    }
    return false;
}

template<typename Key, typename Value>
Value LruCache<Key, Value>::get(Key key)
{
    Value value{};
    // memset(&value, 0, sizeof(value));   // memset 是按字节设置内存的，对于复杂类型（如 string）使用 memset 可能会破坏对象的内部结构
    get(key, value);
    return value;
}

template<typename Key, typename Value>
std::string LruCache<Key, Value>::model() const
{
    return "LRU";
}

template<typename Key, typename Value>
void LruCache<Key, Value>::remove(Key key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodeMap_.find(key);
    if (it != nodeMap_.end())
    {
        removeNode(it->second);
        nodeMap_.erase(it);
    }    
}

template<typename Key, typename Value>
void LruCache<Key, Value>::initializeList()
{
    // 创建首尾虚拟节点
    dummyHead_ = std::make_shared<LruNodeType>(Key(), Value());
    dummyTail_ = std::make_shared<LruNodeType>(Key(), Value());
    dummyHead_->next_ = dummyTail_;
    dummyTail_->prev_ = dummyHead_;
}

template<typename Key, typename Value>
void LruCache<Key, Value>::updateExistingNode(NodePtr node, const Value& value) 
{
    node->setValue(value);
    moveToMostRecent(node);
}

template<typename Key, typename Value>
void LruCache<Key, Value>::addNewNode(const Key& key, const Value& value)
{
    if (nodeMap_.size() >= capacity_) 
    {
        evictLeastRecent();
    }

    NodePtr newNode = std::make_shared<LruNodeType>(key, value);
    insertNode(newNode);
    nodeMap_[key] = newNode;
}

template<typename Key, typename Value>
void LruCache<Key, Value>::moveToMostRecent(NodePtr node)
{
    removeNode(node);
    insertNode(node);
}

template<typename Key, typename Value>
void LruCache<Key, Value>::removeNode(NodePtr node)
{
    if(!node->prev_.expired() && node->next_) 
    {
        auto prev = node->prev_.lock(); // 使用lock()获取shared_ptr
        prev->next_ = node->next_;
        node->next_->prev_ = prev;
        node->next_ = nullptr; // 清空next_指针，彻底断开节点与链表的连接
    }
}

template<typename Key, typename Value>
void LruCache<Key, Value>::insertNode(NodePtr node)
{
    node->next_ = dummyTail_;
    node->prev_ = dummyTail_->prev_;
    dummyTail_->prev_.lock()->next_ = node; // 使用lock()获取shared_ptr
    dummyTail_->prev_ = node;
}

template<typename Key, typename Value>
void LruCache<Key, Value>::evictLeastRecent()
{
    NodePtr leastRecent = dummyHead_->next_;
    removeNode(leastRecent);
    nodeMap_.erase(leastRecent->getKey());
}

}
