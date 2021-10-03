#pragma once

#include <vector>

namespace coel {

template <typename T>
class Stack {
    std::vector<T> m_storage;

public:
    template <typename... Args>
    void emplace(Args &&...args) {
        m_storage.emplace_back(std::forward<Args>(args)...);
    }
    void push(const T &elem) { m_storage.push_back(elem); }
    void push(T &&elem) { m_storage.push_back(std::move(elem)); }
    T &peek() { return m_storage.back(); }
    const T &peek() const { return m_storage.back(); }
    T pop() {
        auto ret = std::move(m_storage.back());
        m_storage.pop_back();
        return std::move(ret);
    }

    bool empty() const { return m_storage.empty(); }
    auto size() const { return m_storage.size(); }
};

} // namespace coel
