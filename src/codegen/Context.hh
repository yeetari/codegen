#pragma once

#include <codegen/Register.hh>

#include <memory>
#include <unordered_map>
#include <vector>

namespace ir {

class Function;

} // namespace ir

namespace codegen {

class Context {
    ir::Function *const m_function;
    std::vector<std::unique_ptr<Register>> m_registers;
    std::size_t m_virtual_count{0};

public:
    explicit Context(ir::Function &function) : m_function(&function) {}

    Register *create_physical(std::size_t phys) { return m_registers.emplace_back(new Register(phys, true)).get(); }
    Register *create_virtual() { return m_registers.emplace_back(new Register(m_virtual_count++, false)).get(); }

    ir::Function *function() const { return m_function; }
};

} // namespace codegen
