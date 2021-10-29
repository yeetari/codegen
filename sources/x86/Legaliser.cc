#include <coel/x86/Legaliser.hh>

#include <coel/codegen/Context.hh>
#include <coel/ir/BasicBlock.hh>
#include <coel/ir/Constant.hh>
#include <coel/ir/Function.hh>
#include <coel/ir/InstVisitor.hh>
#include <coel/ir/Instructions.hh>
#include <coel/ir/Unit.hh>
#include <coel/x86/Register.hh>

namespace coel::x86 {
namespace {

class Legaliser final : public ir::InstVisitor {
    codegen::Context &m_context;
    ir::BasicBlock *m_block{nullptr};

public:
    explicit Legaliser(codegen::Context &context) : m_context(context) {}

    void run(ir::Function *function);
    void visit(ir::BinaryInst *) override;
    void visit(ir::BranchInst *) override {}
    void visit(ir::CallInst *) override;
    void visit(ir::CompareInst *) override;
    void visit(ir::CondBranchInst *) override;
    void visit(ir::CopyInst *) override {}
    void visit(ir::LoadInst *) override {}
    void visit(ir::RetInst *) override;
    void visit(ir::StoreInst *) override;
};

void Legaliser::run(ir::Function *function) {
    for (auto *block : *function) {
        m_block = block;
        for (auto *inst : *block) {
            inst->accept(this);
        }
    }
}

void Legaliser::visit(ir::BinaryInst *binary) {
    auto *lhs_copy = m_context.create_virtual(binary->type());
    m_block->insert<ir::CopyInst>(binary, lhs_copy, binary->lhs());
    binary->set_lhs(lhs_copy);
    binary->replace_all_uses_with(lhs_copy);
}

void Legaliser::visit(ir::CallInst *call) {
    // TODO: Should be handled by register allocator.
    std::array argument_registers{Register::rdi, Register::rsi, Register::rdx,
                                  Register::rcx, Register::r8,  Register::r9};
    for (std::size_t i = 0; i < call->args().size(); i++) {
        auto *arg = call->args()[i];
        auto *phys = m_context.create_physical(arg->type(), argument_registers[i]);
        m_block->insert<ir::CopyInst>(call, phys, arg);
    }
    auto *copy = m_context.create_virtual(call->type());
    m_block->insert<ir::CopyInst>(++m_block->iterator(call), copy, m_context.create_physical(call->type(), 0));
    call->replace_all_uses_with(copy);
}

void Legaliser::visit(ir::CompareInst *compare) {
    if (compare->lhs()->is<codegen::Register>()) {
        compare->replace_all_uses_with(compare->lhs());
        return;
    }
    auto *lhs_copy = m_context.create_virtual(compare->lhs()->type());
    m_block->insert<ir::CopyInst>(compare, lhs_copy, compare->lhs());
    compare->set_lhs(lhs_copy);
    compare->replace_all_uses_with(lhs_copy);
}

void Legaliser::visit(ir::CondBranchInst *cond_branch) {
    if (cond_branch->cond()->is<codegen::Register>()) {
        return;
    }
    auto *cond_copy = m_context.create_virtual(cond_branch->cond()->type());
    m_block->insert<ir::CopyInst>(cond_branch, cond_copy, cond_branch->cond());
    cond_branch->set_cond(cond_copy);
}

void Legaliser::visit(ir::RetInst *ret) {
    auto *value_copy = m_context.create_physical(ret->value()->type(), Register::rax);
    m_block->insert<ir::CopyInst>(ret, value_copy, ret->value());
    ret->set_value(value_copy);
}

void Legaliser::visit(ir::StoreInst *store) {
    if (store->value()->is<ir::Constant>()) {
        return;
    }
    auto *value_copy = m_context.create_virtual(store->value()->type());
    m_block->insert<ir::CopyInst>(store, value_copy, store->value());
    store->set_value(value_copy);
}

} // namespace

void legalise(codegen::Context &context) {
    Legaliser legaliser(context);
    for (auto *function : context.unit()) {
        legaliser.run(function);
    }
}

} // namespace coel::x86
