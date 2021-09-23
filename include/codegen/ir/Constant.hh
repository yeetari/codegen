#pragma once

#include <codegen/ir/Value.hh>

#include <cstdint>

namespace ir {

class Constant final : public Value {
    const std::size_t m_value;

    explicit Constant(std::size_t value) : Value(ValueKind::Constant), m_value(value) {}

public:
    static Constant *get(std::size_t value);

    std::size_t value() const { return m_value; }
};

} // namespace ir
