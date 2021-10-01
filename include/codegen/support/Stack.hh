#pragma once

#include <vector>

template <typename T>
class Stack {
    std::vector<T> m_storage;

public:
    void push(const T &elem) { m_storage.push_back(elem); }
    void push(T &&elem) { m_storage.push_back(std::move(elem)); }
    const T &peek() const { return m_storage.back(); }
    T pop() {
        auto ret = std::move(m_storage.back());
        m_storage.pop_back();
        return std::move(ret);
    }

    bool empty() const { return m_storage.empty(); }
    auto size() const { return m_storage.size(); }
};
