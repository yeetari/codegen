#include <codegen/ir/Function.hh>

namespace ir {

BasicBlock *Function::append_block() {
    return m_blocks.emplace_back(new BasicBlock).get();
}

StackSlot *Function::append_stack_slot() {
    return m_stack_slots.emplace_back(new StackSlot).get();
}

} // namespace ir
