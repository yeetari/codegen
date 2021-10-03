#pragma once

#include <coel/ir/Value.hh>

namespace coel::ir {

class Argument : public Value {
public:
    Argument() : Value(ValueKind::Argument) {}
};

} // namespace coel::ir
