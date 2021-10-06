#pragma once

#include <coel/ir/Value.hh>
#include <coel/support/ListNode.hh>

namespace coel::ir {

class StackSlot : public Value, public ListNode {
public:
    explicit StackSlot(const Type *type);
};

} // namespace coel::ir
