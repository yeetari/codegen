#pragma once

#include <ir/Argument.hh>
#include <ir/BasicBlock.hh>
#include <ir/Value.hh>

#include <memory>
#include <string>
#include <vector>

namespace ir {

class FunctionIterator;

class Function final : public Value {
    const std::string m_name;
    std::vector<Argument> m_arguments;
    std::vector<std::unique_ptr<BasicBlock>> m_blocks;

public:
    Function(std::string &&name, std::size_t argument_count)
        : Value(ValueKind::Function), m_name(std::move(name)), m_arguments(argument_count) {}

    FunctionIterator begin() const;
    FunctionIterator end() const;

    BasicBlock *append_block();
    Argument *argument(std::size_t index) { return &m_arguments[index]; }
    const Argument *argument(std::size_t index) const { return &m_arguments[index]; }

    const std::string &name() const { return m_name; }
    const std::vector<Argument> &arguments() const { return m_arguments; }
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
