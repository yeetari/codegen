#include <coel/ir/Types.hh>

#include <memory>
#include <vector>

namespace coel::ir {
namespace {

const BoolType s_bool_type;
const IntegerType s_int8_type(8);
const IntegerType s_int16_type(16);
const IntegerType s_int32_type(32);
const IntegerType s_int64_type(64);
std::vector<std::unique_ptr<ir::PointerType>> s_pointer_types;

} // namespace

const BoolType *BoolType::get() {
    return &s_bool_type;
}

const IntegerType *IntegerType::get(unsigned int bit_width) {
    switch (bit_width) {
    case 8:
        return &s_int8_type;
    case 16:
        return &s_int16_type;
    case 32:
        return &s_int32_type;
    case 64:
        return &s_int64_type;
    }
    COEL_ENSURE_NOT_REACHED();
}

const PointerType *PointerType::get(const Type *pointee_type) {
    return s_pointer_types.emplace_back(new PointerType(pointee_type)).get();
}

} // namespace coel::ir
