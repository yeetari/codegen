#pragma once

#include <codegen/ir/Value.hh>

namespace ir {

class Argument : public Value {
public:
    Argument() : Value(ValueKind::Argument) {}
};

} // namespace ir
