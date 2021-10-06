#pragma once

#include <coel/ir/Value.hh>

#include <cstddef>

namespace coel::codegen {

class Register : public ir::Value {
    static constexpr std::size_t k_physical_bit = (1ull << (sizeof(std::size_t) * 8 - 1));
    std::size_t m_value;

public:
    Register(const ir::Type *type, std::size_t reg, bool physical)
        : ir::Value(ir::ValueKind::Register, type), m_value(reg | (physical ? k_physical_bit : 0)) {}

    void set_reg(std::size_t reg) { m_value = reg | (physical() ? k_physical_bit : 0); }
    void set_physical(bool physical) { m_value ^= (-physical ^ m_value) & k_physical_bit; }

    std::size_t reg() const { return m_value & ~k_physical_bit; }
    bool physical() const { return (m_value & k_physical_bit) != 0; }
};

} // namespace coel::codegen
