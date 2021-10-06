#include <coel/ir/Function.hh>

namespace coel::ir {

Function::Function(std::string &&name, const Type *return_type, std::span<const Type *> parameters)
    : Value(ValueKind::Function, return_type), m_name(std::move(name)) {
    for (const auto *parameter : parameters) {
        m_arguments.emplace_back(parameter);
    }
}

BasicBlock *Function::append_block() {
    return m_blocks.emplace<BasicBlock>(m_blocks.end());
}

StackSlot *Function::append_stack_slot(const Type *type) {
    return m_stack_slots.emplace<StackSlot>(m_stack_slots.end(), type);
}

} // namespace coel::ir
