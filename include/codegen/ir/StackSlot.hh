#pragma once

#include <codegen/ir/Value.hh>

namespace ir {

class StackSlot : public Value {
public:
    StackSlot() : Value(ValueKind::StackSlot) {}
};

} // namespace ir
