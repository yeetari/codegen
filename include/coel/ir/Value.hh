#pragma once

#include <coel/support/Assert.hh>

#include <unordered_set>

namespace coel::ir {

enum class ValueKind {
    Argument,
    BasicBlock,
    Constant,
    Function,
    Instruction,
    Register,
    StackSlot,
};

// TODO: Constants and Registers shouldn't have a users vector; create a separate Usable class that
//       BasicBlock/Instruction inherit from.
class Value {
    const ValueKind m_kind;
    std::unordered_set<Value *> m_users;

protected:
    explicit Value(ValueKind kind) : m_kind(kind) {}

public:
    Value(const Value &) = delete;
    Value(Value &&) = delete;
    virtual ~Value();

    Value &operator=(const Value &) = delete;
    Value &operator=(Value &&) = delete;

    void add_user(Value *user);
    void remove_user(Value *user);
    void replace_all_uses_with(Value *repl);
    virtual void replace_uses_of_with(Value *orig, Value *repl);

    // TODO: Don't use dynamic_cast, use kind enum instead.
    template <typename T>
    T *as() {
        return dynamic_cast<T *>(this);
    }

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

    ValueKind kind() const { return m_kind; }
};

} // namespace coel::ir
