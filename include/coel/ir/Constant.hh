#pragma once

#include <coel/ir/Value.hh>

#include <cstdint>

namespace coel::ir {

class Constant final : public Value {
    const std::size_t m_value;

    Constant(const Type *type, std::size_t value) : Value(ValueKind::Constant, type), m_value(value) {}

public:
    static Constant *get(const Type *type, std::size_t value);

    std::size_t value() const { return m_value; }
};

} // namespace coel::ir
