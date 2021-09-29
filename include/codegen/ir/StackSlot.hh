#pragma once

#include <codegen/ir/Value.hh>
#include <codegen/support/ListNode.hh>

namespace ir {

class StackSlot : public Value, public ListNode {
public:
    StackSlot() : Value(ValueKind::StackSlot) {}
};

} // namespace ir
