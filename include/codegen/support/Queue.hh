#pragma once

#include <vector>

template <typename T>
class Queue {
    std::vector<T> m_storage;

public:
    void push(T elem) {
        m_storage.insert(m_storage.begin(), elem);
    }

    T pop() {
        auto ret = m_storage.back();
        m_storage.pop_back();
        return ret;
    }
};
