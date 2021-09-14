#include <ir/Instructions.hh>

#include <ir/BasicBlock.hh>
#include <ir/InstVisitor.hh>
#include <support/Assert.hh>

namespace ir {

AddInst::AddInst(Value *lhs, Value *rhs) : Instruction(Opcode::Add), m_lhs(lhs), m_rhs(rhs) {
    lhs->add_user(this);
    rhs->add_user(this);
}

AddInst::~AddInst() {
    if (m_lhs != nullptr) {
        m_lhs->remove_user(this);
    }
    if (m_rhs != nullptr) {
        m_rhs->remove_user(this);
    }
}

void AddInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void AddInst::replace_uses_of_with(Value *orig, Value *repl) {
    if (m_lhs == orig) {
        m_lhs->remove_user(this);
        m_lhs = repl;
        if (m_lhs != nullptr) {
            m_lhs->add_user(this);
        }
    }
    if (m_rhs == orig) {
        m_rhs->remove_user(this);
        m_rhs = repl;
        if (m_rhs != nullptr) {
            m_rhs->add_user(this);
        }
    }
}

void AddInst::set_lhs(Value *lhs) {
    ASSERT(lhs != nullptr);
    m_lhs->remove_user(this);
    m_lhs = lhs;
    m_lhs->add_user(this);
}

BranchInst::BranchInst(BasicBlock *dst) : Instruction(Opcode::Branch), m_dst(dst) {
    dst->add_user(this);
}

BranchInst::~BranchInst() {
    m_dst->remove_user(this);
}

void BranchInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

CondBranchInst::CondBranchInst(Value *cond, BasicBlock *true_dst, BasicBlock *false_dst)
    : Instruction(Opcode::CondBranch), m_cond(cond), m_true_dst(true_dst), m_false_dst(false_dst) {
    cond->add_user(this);
    true_dst->add_user(this);
    false_dst->add_user(this);
}

CondBranchInst::~CondBranchInst() {
    if (m_cond != nullptr) {
        m_cond->remove_user(this);
    }
    m_true_dst->remove_user(this);
    m_false_dst->remove_user(this);
}

void CondBranchInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void CondBranchInst::replace_uses_of_with(Value *orig, Value *repl) {
    ASSERT(orig != m_true_dst);
    ASSERT(orig != m_false_dst);
    if (m_cond == orig) {
        m_cond->remove_user(this);
        m_cond = repl;
        if (m_cond != nullptr) {
            m_cond->add_user(this);
        }
    }
}

void CondBranchInst::set_cond(Value *cond) {
    ASSERT(cond != nullptr);
    m_cond->remove_user(this);
    m_cond = cond;
    m_cond->add_user(this);
}

CopyInst::CopyInst(codegen::Register *dst, Value *src) : Instruction(Opcode::Copy), m_dst(dst), m_src(src) {
    src->add_user(this);
}

CopyInst::~CopyInst() {
    if (m_src != nullptr) {
        m_src->remove_user(this);
    }
}

void CopyInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void CopyInst::replace_uses_of_with(Value *orig, Value *repl) {
    ASSERT(m_dst != orig);
    if (m_src == orig) {
        m_src->remove_user(this);
        m_src = repl;
        if (m_src != nullptr) {
            m_src->add_user(this);
        }
    }
}

RetInst::RetInst(Value *value) : Instruction(Opcode::Ret), m_value(value) {
    value->add_user(this);
}

RetInst::~RetInst() {
    if (m_value != nullptr) {
        m_value->remove_user(this);
    }
}

void RetInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void RetInst::replace_uses_of_with(Value *orig, Value *repl) {
    if (m_value == orig) {
        m_value->remove_user(this);
        m_value = repl;
        if (m_value != nullptr) {
            m_value->add_user(this);
        }
    }
}

void RetInst::set_value(Value *value) {
    ASSERT(value != nullptr);
    m_value->remove_user(this);
    m_value = value;
    m_value->add_user(this);
}

} // namespace ir
