#pragma once

#include <coel/ir/Type.hh>

namespace coel::ir {

class BoolType : public Type {
public:
    static const BoolType *get();

    BoolType() : Type(TypeKind::Bool) {}
};

class IntegerType : public Type {
    const unsigned m_bit_width;

public:
    static const IntegerType *get(unsigned bit_width);

    explicit IntegerType(unsigned bit_width) : Type(TypeKind::Integer), m_bit_width(bit_width) {}

    unsigned bit_width() const { return m_bit_width; }
};

class PointerType : public Type {
    const Type *const m_pointee_type;

public:
    static const PointerType *get(const Type *pointee_type);

    explicit PointerType(const Type *pointee_type) : Type(TypeKind::Pointer), m_pointee_type(pointee_type) {}

    const Type *pointee_type() const { return m_pointee_type; }
};

} // namespace coel::ir
