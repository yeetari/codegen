#pragma once

#include <ir/BasicBlock.hh>

#include <memory>
#include <vector>

namespace ir {

class FunctionIterator;

class Function {
    std::vector<std::unique_ptr<BasicBlock>> m_blocks;

public:
    FunctionIterator begin() const;
    FunctionIterator end() const;

    BasicBlock *append_block();
};

class FunctionIterator {
    const std::vector<std::unique_ptr<BasicBlock>> *m_blocks;
    std::size_t m_index;

public:
    FunctionIterator(const std::vector<std::unique_ptr<BasicBlock>> &blocks, std::size_t index)
        : m_blocks(&blocks), m_index(index) {}

    FunctionIterator &operator++() {
        m_index++;
        return *this;
    }
    FunctionIterator &operator--() {
        m_index--;
        return *this;
    }

    bool operator<=>(const FunctionIterator &) const = default;
    BasicBlock *operator*() const { return (*m_blocks)[m_index].get(); }
    std::size_t index() const { return m_index; }
};

inline FunctionIterator Function::begin() const {
    return {m_blocks, 0};
}

inline FunctionIterator Function::end() const {
    return {m_blocks, m_blocks.size()};
}

} // namespace ir
