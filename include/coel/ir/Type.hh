#pragma once

#include <coel/support/Assert.hh>

namespace coel::ir {

enum class TypeKind {
    Bool,
    Integer,
    Pointer,
};

class Type {
    const TypeKind m_kind;

protected:
    explicit Type(TypeKind kind) : m_kind(kind) {}

public:
    Type(const Type &) = delete;
    Type(Type &&) = delete;
    virtual ~Type() = default;

    Type &operator=(const Type &) = delete;
    Type &operator=(Type &&) = delete;

    // TODO: Don't use dynamic_cast, use kind enum instead.
    template <typename T>
    const T *as() const {
        return dynamic_cast<const T *>(this);
    }

    template <typename T>
    const T *as_non_null() const {
        COEL_ASSERT(is<T>());
        COEL_ASSERT_PEDANTIC(dynamic_cast<const T *>(this) != nullptr);
        return static_cast<const T *>(this);
    }

    template <typename T>
    bool is() const {
        return as<T>() != nullptr;
    }

    TypeKind kind() const { return m_kind; }
};

} // namespace coel::ir
