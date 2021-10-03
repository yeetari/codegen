#include <coel/ir/Function.hh>

namespace coel::ir {

BasicBlock *Function::append_block() {
    return m_blocks.emplace<BasicBlock>(m_blocks.end());
}

StackSlot *Function::append_stack_slot() {
    return m_stack_slots.emplace<StackSlot>(m_stack_slots.end());
}

} // namespace coel::ir
