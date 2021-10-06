#pragma once

#include <coel/ir/Value.hh>

namespace coel::ir {

class Argument : public Value {
public:
    explicit Argument(const Type *type) : Value(ValueKind::Argument, type) {}
};

} // namespace coel::ir
