#pragma once

#include <coel/ir/Argument.hh>
#include <coel/ir/BasicBlock.hh>
#include <coel/ir/StackSlot.hh>
#include <coel/ir/Value.hh>
#include <coel/support/List.hh>
#include <coel/support/ListNode.hh>

#include <memory>
#include <span>
#include <string>
#include <vector>

namespace coel::ir {

class Function final : public Value, public ListNode {
    const std::string m_name;
    std::vector<Argument> m_arguments;
    List<BasicBlock> m_blocks;
    List<StackSlot> m_stack_slots;

public:
    Function(std::string &&name, const Type *return_type, std::span<const Type *> parameters);

    auto begin() const { return m_blocks.begin(); }
    auto end() const { return m_blocks.end(); }

    BasicBlock *append_block();
    StackSlot *append_stack_slot(const Type *type);
    Argument *argument(std::size_t index) { return &m_arguments[index]; }
    const Argument *argument(std::size_t index) const { return &m_arguments[index]; }

    const std::string &name() const { return m_name; }
    const std::vector<Argument> &arguments() const { return m_arguments; }
    const List<StackSlot> &stack_slots() const { return m_stack_slots; }
};

} // namespace coel::ir
