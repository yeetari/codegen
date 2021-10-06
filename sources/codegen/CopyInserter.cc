#include <coel/codegen/CopyInserter.hh>

#include <coel/codegen/Context.hh>
#include <coel/ir/BasicBlock.hh>
#include <coel/ir/Function.hh>
#include <coel/ir/InstVisitor.hh>
#include <coel/ir/Instructions.hh>
#include <coel/ir/Unit.hh>
#include <coel/support/Assert.hh>

namespace coel::codegen {
namespace {

class CopyInserter final : public ir::InstVisitor {
    Context &m_context;
    ir::BasicBlock *m_block{nullptr};

public:
    explicit CopyInserter(Context &context) : m_context(context) {}

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

void CopyInserter::run(ir::Function *function) {
    for (auto *block : *function) {
        m_block = block;
        for (auto *inst : *block) {
            inst->accept(this);
        }
    }
}

void CopyInserter::visit(ir::BinaryInst *binary) {
    auto *copy = m_context.create_virtual(binary->type());
    m_block->insert<ir::CopyInst>(binary, copy, binary->lhs());
    binary->set_lhs(copy);
    binary->replace_all_uses_with(copy);
}

void CopyInserter::visit(ir::CallInst *call) {
    // TODO: Assuming target/ABI registers.
    std::array argument_registers{7, 6, 2, 1, 8, 9};
    for (std::size_t i = 0; i < call->args().size(); i++) {
        auto *arg = call->args()[i];
        auto *phys = m_context.create_physical(arg->type(), argument_registers[i]);
        m_block->insert<ir::CopyInst>(call, phys, arg);
    }
    auto *copy = m_context.create_virtual(call->type());
    m_block->insert<ir::CopyInst>(++m_block->iterator(call), copy, m_context.create_physical(call->type(), 0));
    call->replace_all_uses_with(copy);
}

void CopyInserter::visit(ir::CompareInst *compare) {
    auto *copy = m_context.create_virtual(compare->lhs()->type());
    m_block->insert<ir::CopyInst>(compare, copy, compare->lhs());
    compare->set_lhs(copy);
    compare->replace_all_uses_with(copy);
}

void CopyInserter::visit(ir::CondBranchInst *cond_branch) {
    auto *copy = m_context.create_virtual(cond_branch->cond()->type());
    m_block->insert<ir::CopyInst>(cond_branch, copy, cond_branch->cond());
    cond_branch->set_cond(copy);
}

void CopyInserter::visit(ir::RetInst *ret) {
    // TODO: Assuming target register 0 is return register.
    auto *copy = m_context.create_physical(ret->value()->type(), 0);
    m_block->insert<ir::CopyInst>(ret, copy, ret->value());
    ret->set_value(copy);
}

void CopyInserter::visit(ir::StoreInst *store) {
    auto *copy = m_context.create_virtual(store->value()->type());
    m_block->insert<ir::CopyInst>(store, copy, store->value());
    store->set_value(copy);
}

} // namespace

void insert_copies(Context &context) {
    CopyInserter inserter(context);
    for (auto *function : context.unit()) {
        inserter.run(function);
    }
}

} // namespace coel::codegen
