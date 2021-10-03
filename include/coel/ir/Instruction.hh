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
    explicit Instruction(Opcode opcode) : Value(ValueKind::Instruction), m_opcode(opcode) {}

public:
    virtual void accept(InstVisitor *visitor) = 0;

    Opcode opcode() const { return m_opcode; }
};

} // namespace coel::ir
