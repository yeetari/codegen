#pragma once

#include <codegen/Register.hh>

#include <memory>
#include <unordered_map>
#include <vector>

namespace ir {

class Unit;

} // namespace ir

namespace codegen {

class Context {
    ir::Unit *const m_unit;
    std::vector<std::unique_ptr<Register>> m_registers;
    std::size_t m_virtual_count{0};

public:
    explicit Context(ir::Unit &unit) : m_unit(&unit) {}

    Register *create_physical(std::size_t phys) { return m_registers.emplace_back(new Register(phys, true)).get(); }
    Register *create_virtual() { return m_registers.emplace_back(new Register(m_virtual_count++, false)).get(); }

    ir::Unit &unit() const { return *m_unit; }
};

} // namespace codegen
