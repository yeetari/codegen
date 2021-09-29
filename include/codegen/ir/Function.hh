#pragma once

#include <codegen/ir/Argument.hh>
#include <codegen/ir/BasicBlock.hh>
#include <codegen/ir/StackSlot.hh>
#include <codegen/ir/Value.hh>
#include <codegen/support/List.hh>
#include <codegen/support/ListNode.hh>

#include <memory>
#include <string>
#include <vector>

namespace ir {

class Function final : public Value, public ListNode {
    const std::string m_name;
    std::vector<Argument> m_arguments;
    List<BasicBlock> m_blocks;
    List<StackSlot> m_stack_slots;

public:
    Function(std::string &&name, std::size_t argument_count)
        : Value(ValueKind::Function), m_name(std::move(name)), m_arguments(argument_count) {}

    auto begin() const { return m_blocks.begin(); }
    auto end() const { return m_blocks.end(); }

    BasicBlock *append_block();
    StackSlot *append_stack_slot();
    Argument *argument(std::size_t index) { return &m_arguments[index]; }
    const Argument *argument(std::size_t index) const { return &m_arguments[index]; }

    const std::string &name() const { return m_name; }
    const std::vector<Argument> &arguments() const { return m_arguments; }
    const List<StackSlot> &stack_slots() const { return m_stack_slots; }
};

} // namespace ir
