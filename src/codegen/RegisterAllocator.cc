#include <codegen/RegisterAllocator.hh>

#include <codegen/Liveness.hh>
#include <codegen/Register.hh>
#include <graph/DominanceComputer.hh>
#include <graph/DominatorTree.hh>
#include <graph/Graph.hh>
#include <ir/Function.hh>
#include <ir/InstVisitor.hh>
#include <ir/Instructions.hh>
#include <support/Assert.hh>
#include <x86/Register.hh>

#include <memory>
#include <unordered_map>
#include <vector>

namespace codegen {
namespace {

class RegisterAllocator final : public ir::InstVisitor {
    const ir::Instruction *m_inst{nullptr};
    std::unique_ptr<Liveness> m_liveness;
    std::unordered_map<std::size_t, Register *> m_matrix;
    std::vector<std::size_t> m_phys_regs;

public:
    RegisterAllocator();

    void allocate(Register *virt);
    void run(ir::Function &function);
    void visit(ir::AddInst *) override;
    void visit(ir::BranchInst *) override;
    void visit(ir::CondBranchInst *) override;
    void visit(ir::CopyInst *) override;
    void visit(ir::RetInst *) override;
};

RegisterAllocator::RegisterAllocator() {
    m_phys_regs.push_back(static_cast<std::size_t>(x86::Register::rsi));
    m_phys_regs.push_back(static_cast<std::size_t>(x86::Register::rdi));
    m_phys_regs.push_back(static_cast<std::size_t>(x86::Register::rax));
    m_phys_regs.push_back(static_cast<std::size_t>(x86::Register::rbx));
    m_phys_regs.push_back(static_cast<std::size_t>(x86::Register::rcx));
    m_phys_regs.push_back(static_cast<std::size_t>(x86::Register::rdx));
}

void RegisterAllocator::allocate(Register *virt) {
    if (virt->physical()) {
        return;
    }
    for (std::size_t phys : m_phys_regs) {
        if (m_matrix[phys] == nullptr) {
            continue;
        }
        bool live = m_liveness->live_at(m_matrix[phys], m_inst);
        if (!live) {
            m_matrix[phys] = nullptr;
        }
    }
    for (std::size_t phys : m_phys_regs) {
        if (m_matrix[phys] != nullptr) {
            continue;
        }
        virt->set_reg(phys);
        virt->set_physical(true);
        m_matrix[phys] = virt;
        return;
    }
    ENSURE_NOT_REACHED("Ran out of registers");
}

void RegisterAllocator::run(ir::Function &function) {
    Graph<ir::BasicBlock> cfg(*function.begin());
    for (auto *block : function) {
        for (auto *inst : *block) {
            if (auto *branch = inst->as<ir::BranchInst>()) {
                cfg.connect(block, branch->dst());
            } else if (auto *cond_branch = inst->as<ir::CondBranchInst>()) {
                cfg.connect(block, cond_branch->true_dst());
                cfg.connect(block, cond_branch->false_dst());
            }
        }
    }
    m_liveness = std::make_unique<Liveness>(function, cfg);

    auto dom_tree = cfg.run<DominanceComputer>();
    auto dom_dfs = dom_tree.run<DepthFirstSearch>();
    for (auto *block : dom_dfs.pre_order()) {
        for (auto *inst : *block) {
            m_inst = inst;
            inst->accept(this);
        }
    }
}

void RegisterAllocator::visit(ir::AddInst *add) {
    if (auto *reg = add->lhs()->as<Register>()) {
        allocate(reg);
    }
    if (auto *reg = add->rhs()->as<Register>()) {
        allocate(reg);
    }
    add->replace_all_uses_with(add->lhs());
}

void RegisterAllocator::visit(ir::BranchInst *) {}

void RegisterAllocator::visit(ir::CondBranchInst *cond_branch) {
    if (auto *reg = cond_branch->cond()->as<Register>()) {
        allocate(reg);
    }
}

void RegisterAllocator::visit(ir::CopyInst *copy) {
    allocate(copy->dst());
}

void RegisterAllocator::visit(ir::RetInst *ret) {
    if (auto *reg = ret->value()->as<Register>()) {
        allocate(reg);
    }
}

} // namespace

void register_allocate(ir::Function &function) {
    RegisterAllocator allocator;
    allocator.run(function);
}

} // namespace codegen
