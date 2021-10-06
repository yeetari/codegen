#pragma once

#include <coel/codegen/Register.hh>

#include <memory>
#include <unordered_map>
#include <vector>

namespace coel::ir {

class Unit;

} // namespace coel::ir

namespace coel::codegen {

class Context {
    ir::Unit *const m_unit;
    std::vector<std::unique_ptr<Register>> m_registers;
    std::size_t m_virtual_count{0};

public:
    explicit Context(ir::Unit &unit) : m_unit(&unit) {}

    Register *create_physical(const ir::Type *type, std::size_t phys) {
        return m_registers.emplace_back(new Register(type, phys, true)).get();
    }
    Register *create_virtual(const ir::Type *type) {
        return m_registers.emplace_back(new Register(type, m_virtual_count++, false)).get();
    }

    ir::Unit &unit() const { return *m_unit; }
};

} // namespace coel::codegen
