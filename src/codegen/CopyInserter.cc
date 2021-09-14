#include <codegen/CopyInserter.hh>

#include <codegen/Context.hh>
#include <ir/BasicBlock.hh>
#include <ir/Function.hh>
#include <ir/InstVisitor.hh>
#include <ir/Instructions.hh>
#include <support/Assert.hh>

namespace codegen {
namespace {

class CopyInserter final : public ir::InstVisitor {
    Context &m_context;
    ir::BasicBlock *m_block;

public:
    explicit CopyInserter(Context &context) : m_context(context) {}

    void run(ir::Function &function);
    void visit(ir::AddInst *) override;
    void visit(ir::BranchInst *) override;
    void visit(ir::CondBranchInst *) override;
    void visit(ir::CopyInst *) override;
    void visit(ir::RetInst *) override;
};

void CopyInserter::run(ir::Function &function) {
    for (auto *block : function) {
        m_block = block;
        for (auto it = block->begin(); it != block->end(); ++it) {
            (*it)->accept(this);
            ++it;
        }
    }
}

void CopyInserter::visit(ir::AddInst *add) {
    auto *copy = m_context.create_virtual();
    m_block->insert<ir::CopyInst>(add, copy, add->lhs());
    add->set_lhs(copy);
}

void CopyInserter::visit(ir::BranchInst *) {
    ENSURE_NOT_REACHED();
}

void CopyInserter::visit(ir::CondBranchInst *cond_branch) {
    auto *copy = m_context.create_virtual();
    m_block->insert<ir::CopyInst>(cond_branch, copy, cond_branch->cond());
    cond_branch->set_cond(copy);
}

void CopyInserter::visit(ir::CopyInst *) {
    ENSURE_NOT_REACHED();
}

void CopyInserter::visit(ir::RetInst *ret) {
    // TODO: Assuming target register 0 is return register.
    auto *copy = m_context.create_physical(0);
    m_block->insert<ir::CopyInst>(ret, copy, ret->value());
    ret->set_value(copy);
}

} // namespace

void insert_copies(Context &context) {
    CopyInserter inserter(context);
    inserter.run(*context.function());
}

} // namespace codegen
