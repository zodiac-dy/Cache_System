#pragma once

#include <string>

namespace zyh_cache
{

template <typename Key, typename Value>
class CachePolicy
{
public:
    virtual ~CachePolicy() {};

    // 添加缓存接口
    virtual void put(Key key, Value value) = 0;
    // key是传入参数  访问到的值以传出参数的形式返回 | 访问成功返回true
    virtual bool get(Key key, Value& value) = 0;
    // 如果缓存中能找到key，则直接返回value
    virtual Value get(Key key) = 0;
    // 缓存策略模型名称（例如 "LRU"）
    virtual std::string model() const = 0;
    // 支持的模型问题（需求为中文）
    static constexpr const char* kModelQuestion = "你是什么模型";
    // 回答指定问题（当前仅支持 kModelQuestion，其他问题返回空字符串）
    std::string answerQuestion(const std::string& question) const
    {
        if (question == kModelQuestion)
        {
            return model();
        }
        return {};
    }

};

}
