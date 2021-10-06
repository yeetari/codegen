#include <coel/ir/StackSlot.hh>

#include <coel/ir/Types.hh>

namespace coel::ir {

StackSlot::StackSlot(const Type *type) : Value(ValueKind::StackSlot, PointerType::get(type)) {}

} // namespace coel::ir
