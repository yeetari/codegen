#pragma once

#include <codegen/graph/Graph.hh>
#include <codegen/ir/InstVisitor.hh>

#include <unordered_map>

namespace ir {

class BasicBlock;
class Function;
class Instruction;
class Value;

} // namespace ir

namespace codegen {

class Liveness final : public ir::InstVisitor {
    const Graph<ir::BasicBlock> *const m_cfg;
    ir::BasicBlock *m_block{nullptr};
    ir::Instruction *m_inst{nullptr};
    std::unordered_map<const ir::Value *, ir::BasicBlock *> m_def_map;
    std::unordered_map<const ir::Instruction *, std::unordered_map<const ir::Value *, bool>> m_live_map;

    void visit_def(const ir::Value *);
    void visit_use(const ir::Value *);

public:
    Liveness(ir::Function &function, const Graph<ir::BasicBlock> &cfg);

    bool live_at(const ir::Value *value, const ir::Instruction *point);
    void visit(ir::AddInst *) override;
    void visit(ir::BranchInst *) override {}
    void visit(ir::CallInst *) override;
    void visit(ir::CondBranchInst *) override;
    void visit(ir::CopyInst *) override;
    void visit(ir::LoadInst *) override;
    void visit(ir::RetInst *) override;
    void visit(ir::StoreInst *) override {}
};

} // namespace codegen
