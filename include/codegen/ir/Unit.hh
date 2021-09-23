#pragma once

#include <codegen/ir/Function.hh>

#include <cstddef>
#include <memory>
#include <string>

namespace ir {

class UnitIterator;

class Unit {
    std::vector<std::unique_ptr<Function>> m_functions;

public:
    UnitIterator begin() const;
    UnitIterator end() const;

    Function *append_function(std::string name, std::size_t argument_count);
};

class UnitIterator {
    const std::vector<std::unique_ptr<Function>> *m_functions;
    std::size_t m_index;

public:
    UnitIterator(const std::vector<std::unique_ptr<Function>> &functions, std::size_t index)
        : m_functions(&functions), m_index(index) {}

    UnitIterator &operator++() {
        m_index++;
        return *this;
    }
    UnitIterator &operator--() {
        m_index--;
        return *this;
    }

    bool operator<=>(const UnitIterator &) const = default;
    Function *operator*() const { return (*m_functions)[m_index].get(); }
    std::size_t index() const { return m_index; }
};

inline UnitIterator Unit::begin() const {
    return {m_functions, 0};
}

inline UnitIterator Unit::end() const {
    return {m_functions, m_functions.size()};
}

} // namespace ir
