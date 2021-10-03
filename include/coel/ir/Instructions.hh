#pragma once

#include <coel/codegen/Register.hh>
#include <coel/ir/Instruction.hh>

#include <vector>

namespace coel::ir {

class BasicBlock;

enum class BinaryOp {
    Add,
    Sub,
};

class BinaryInst final : public Instruction {
    const BinaryOp m_op;
    Value *m_lhs;
    Value *m_rhs;

public:
    BinaryInst(BinaryOp op, Value *lhs, Value *rhs);
    BinaryInst(const BinaryInst &) = delete;
    BinaryInst(BinaryInst &&) = delete;
    ~BinaryInst() override;

    BinaryInst &operator=(const BinaryInst &) = delete;
    BinaryInst &operator=(BinaryInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_lhs(Value *lhs);

    BinaryOp op() const { return m_op; }
    Value *lhs() const { return m_lhs; }
    Value *rhs() const { return m_rhs; }
};

class BranchInst final : public Instruction {
    BasicBlock *m_dst;

public:
    explicit BranchInst(BasicBlock *dst);
    BranchInst(const BranchInst &) = delete;
    BranchInst(BranchInst &&) = delete;
    ~BranchInst() override;

    BranchInst &operator=(const BranchInst &) = delete;
    BranchInst &operator=(BranchInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    BasicBlock *dst() const { return m_dst; }
};

class CallInst final : public Instruction {
    Value *m_callee;
    std::vector<Value *> m_args;

public:
    CallInst(Value *callee, std::vector<Value *> &&args);
    CallInst(const CallInst &) = delete;
    CallInst(CallInst &&) = delete;
    ~CallInst() override;

    CallInst &operator=(const CallInst &) = delete;
    CallInst &operator=(CallInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *callee() const { return m_callee; }
    const std::vector<Value *> &args() const { return m_args; }
};

enum class CompareOp {
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,
};

class CompareInst final : public Instruction {
    const CompareOp m_op;
    Value *m_lhs;
    Value *m_rhs;

public:
    CompareInst(CompareOp op, Value *lhs, Value *rhs);
    CompareInst(const CompareInst &) = delete;
    CompareInst(CompareInst &&) = delete;
    ~CompareInst() override;

    CompareInst &operator=(const CompareInst &) = delete;
    CompareInst &operator=(CompareInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_lhs(Value *lhs);

    CompareOp op() const { return m_op; }
    Value *lhs() const { return m_lhs; }
    Value *rhs() const { return m_rhs; }
};

class CondBranchInst final : public Instruction {
    Value *m_cond;
    BasicBlock *m_true_dst;
    BasicBlock *m_false_dst;

public:
    CondBranchInst(Value *cond, BasicBlock *true_dst, BasicBlock *false_dst);
    CondBranchInst(const CondBranchInst &) = delete;
    CondBranchInst(CondBranchInst &&) = delete;
    ~CondBranchInst() override;

    CondBranchInst &operator=(const CondBranchInst &) = delete;
    CondBranchInst &operator=(CondBranchInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_cond(Value *cond);

    Value *cond() const { return m_cond; }
    BasicBlock *true_dst() const { return m_true_dst; }
    BasicBlock *false_dst() const { return m_false_dst; }
};

class CopyInst final : public Instruction {
    codegen::Register *m_dst;
    Value *m_src;

public:
    CopyInst(codegen::Register *dst, Value *src);
    CopyInst(const CopyInst &) = delete;
    CopyInst(CopyInst &&) = delete;
    ~CopyInst() override;

    CopyInst &operator=(const CopyInst &) = delete;
    CopyInst &operator=(CopyInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    codegen::Register *dst() const { return m_dst; }
    Value *src() const { return m_src; }
};

class LoadInst final : public Instruction {
    Value *m_ptr;

public:
    explicit LoadInst(Value *ptr);
    LoadInst(const LoadInst &) = delete;
    LoadInst(LoadInst &&) = delete;
    ~LoadInst() override;

    LoadInst &operator=(const LoadInst &) = delete;
    LoadInst &operator=(LoadInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;

    Value *ptr() const { return m_ptr; }
};

class RetInst final : public Instruction {
    Value *m_value;

public:
    explicit RetInst(Value *value);
    RetInst(const RetInst &) = delete;
    RetInst(RetInst &&) = delete;
    ~RetInst() override;

    RetInst &operator=(const RetInst &) = delete;
    RetInst &operator=(RetInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_value(Value *value);

    Value *value() const { return m_value; }
};

class StoreInst final : public Instruction {
    Value *m_ptr;
    Value *m_value;

public:
    StoreInst(Value *ptr, Value *value);
    StoreInst(const StoreInst &) = delete;
    StoreInst(StoreInst &&) = delete;
    ~StoreInst() override;

    StoreInst &operator=(const StoreInst &) = delete;
    StoreInst &operator=(StoreInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_value(Value *value);

    Value *ptr() const { return m_ptr; }
    Value *value() const { return m_value; }
};

} // namespace coel::ir
