#pragma once

#include <coel/ir/Value.hh>
#include <coel/support/ListNode.hh>

namespace coel::ir {

class InstVisitor;

enum class Opcode {
    Binary,
    Branch,
    Call,
    Compare,
    CondBranch,
    Copy,
    Load,
    Ret,
    Store,
};

class Instruction : public Value, public ListNode {
    const Opcode m_opcode;

protected:
    Instruction(Opcode opcode, const Type *type) : Value(ValueKind::Instruction, type), m_opcode(opcode) {}

public:
    // TODO: Maybe declare the implementations of this inline in the header?
    virtual void accept(InstVisitor *visitor) = 0;
    virtual bool is_terminator() const = 0;

    Opcode opcode() const { return m_opcode; }
};

} // namespace coel::ir
