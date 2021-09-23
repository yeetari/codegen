#pragma once

#include <ir/Value.hh>

namespace ir {

class InstVisitor;

enum class Opcode {
    Add,
    Branch,
    Call,
    CondBranch,
    Copy,
    Ret,
};

class Instruction : public Value {
    const Opcode m_opcode;

protected:
    explicit Instruction(Opcode opcode) : Value(ValueKind::Instruction), m_opcode(opcode) {}

public:
    virtual void accept(InstVisitor *visitor) = 0;

    Opcode opcode() const { return m_opcode; }
};

} // namespace ir
