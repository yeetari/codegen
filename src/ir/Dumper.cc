#include <ir/Dumper.hh>

#include <ir/BasicBlock.hh>
#include <ir/Constant.hh>
#include <ir/Function.hh>
#include <ir/InstVisitor.hh>
#include <ir/Instructions.hh>
#include <support/Assert.hh>

#include <fmt/core.h>

#include <string>
#include <unordered_map>

namespace ir {
namespace {

class Dumper final : public InstVisitor {
    std::unordered_map<BasicBlock *, std::size_t> m_block_map;
    std::unordered_map<Value *, std::size_t> m_value_map;

    std::string value_string(Value *value);

public:
    void dump(BasicBlock &block);
    void visit(AddInst *) override;
    void visit(BranchInst *) override;
    void visit(CondBranchInst *) override;
    void visit(CopyInst *) override;
    void visit(RetInst *) override;
};

std::string Dumper::value_string(Value *value) {
    if (auto *block = value->as<BasicBlock>()) {
        if (!m_block_map.contains(block)) {
            m_block_map.emplace(block, m_block_map.size());
        }
        return fmt::format("L{}", m_block_map.at(block));
    }
    if (auto *constant = value->as<Constant>()) {
        return std::to_string(constant->value());
    }
    if (auto *reg = value->as<codegen::Register>()) {
        return fmt::format("{}({})", reg->physical() ? "phys" : "virt", reg->reg());
    }
    if (!m_value_map.contains(value)) {
        m_value_map.emplace(value, m_value_map.size());
    }
    return fmt::format("%v{}", m_value_map.at(value));
}

void Dumper::dump(BasicBlock &block) {
    fmt::print("{} {{\n", value_string(&block));
    for (auto *inst : block) {
        fmt::print("  ");
        // TODO: inst->is_terminator()
        if (!inst->is<BranchInst>() && !inst->is<CondBranchInst>() && !inst->is<CopyInst>() && !inst->is<RetInst>()) {
            fmt::print("{} = ", value_string(inst));
        }
        inst->accept(this);
        fmt::print("\n");
    }
    fmt::print("}}\n");
}

void Dumper::visit(AddInst *add) {
    fmt::print("add {}, {}", value_string(add->lhs()), value_string(add->rhs()));
}

void Dumper::visit(BranchInst *) {
    ENSURE_NOT_REACHED();
}

void Dumper::visit(CondBranchInst *cond_branch) {
    fmt::print("br {}, {}, {}", value_string(cond_branch->cond()), value_string(cond_branch->true_dst()),
               value_string(cond_branch->false_dst()));
}

void Dumper::visit(CopyInst *copy) {
    fmt::print("copy {}, {}", value_string(copy->dst()), value_string(copy->src()));
}

void Dumper::visit(RetInst *ret) {
    fmt::print("ret {}", value_string(ret->value()));
}

} // namespace

void dump(Function &function) {
    Dumper dumper;
    for (auto *block : function) {
        dumper.dump(*block);
    }
}

} // namespace ir
