#pragma once

#include <codegen/codegen/Register.hh>
#include <codegen/ir/Instruction.hh>

#include <vector>

namespace ir {

class BasicBlock;

class AddInst final : public Instruction {
    Value *m_lhs;
    Value *m_rhs;

public:
    AddInst(Value *lhs, Value *rhs);
    AddInst(const AddInst &) = delete;
    AddInst(AddInst &&) = delete;
    ~AddInst() override;

    AddInst &operator=(const AddInst &) = delete;
    AddInst &operator=(AddInst &&) = delete;

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_lhs(Value *lhs);

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

} // namespace ir
