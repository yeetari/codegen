#include <codegen/ir/Dumper.hh>

#include <codegen/ir/BasicBlock.hh>
#include <codegen/ir/Constant.hh>
#include <codegen/ir/Function.hh>
#include <codegen/ir/InstVisitor.hh>
#include <codegen/ir/Instructions.hh>
#include <codegen/ir/Unit.hh>
#include <codegen/support/Assert.hh>

#include <fmt/core.h>

#include <string>
#include <unordered_map>

namespace ir {
namespace {

class Dumper final : public InstVisitor {
    const Function *const m_function;
    std::unordered_map<BasicBlock *, std::size_t> m_block_map;
    std::unordered_map<Value *, std::size_t> m_value_map;

    std::string value_string(Value *value);

public:
    explicit Dumper(const Function *function) : m_function(function) {}

    void dump(BasicBlock &block);
    void visit(AddInst *) override;
    void visit(BranchInst *) override;
    void visit(CallInst *) override;
    void visit(CondBranchInst *) override;
    void visit(CopyInst *) override;
    void visit(RetInst *) override;
};

std::string Dumper::value_string(Value *value) {
    if (auto *argument = value->as<Argument>()) {
        auto it = std::find_if(m_function->arguments().begin(), m_function->arguments().end(),
                               [argument](const Argument &arg) {
                                   return &arg == argument;
                               });
        auto index = std::distance(m_function->arguments().begin(), it);
        return fmt::format("%a{}", index);
    }
    if (auto *block = value->as<BasicBlock>()) {
        if (!m_block_map.contains(block)) {
            m_block_map.emplace(block, m_block_map.size());
        }
        return fmt::format("L{}", m_block_map.at(block));
    }
    if (auto *constant = value->as<Constant>()) {
        return std::to_string(constant->value());
    }
    if (auto *function = value->as<Function>()) {
        return fmt::format("@{}", function->name());
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
    fmt::print("  {} {{\n", value_string(&block));
    for (auto *inst : block) {
        fmt::print("    ");
        // TODO: inst->is_terminator()
        if (!inst->is<BranchInst>() && !inst->is<CondBranchInst>() && !inst->is<CopyInst>() && !inst->is<RetInst>()) {
            fmt::print("{} = ", value_string(inst));
        }
        inst->accept(this);
        fmt::print("\n");
    }
    fmt::print("  }}\n");
}

void Dumper::visit(AddInst *add) {
    fmt::print("add {}, {}", value_string(add->lhs()), value_string(add->rhs()));
}

void Dumper::visit(BranchInst *branch) {
    fmt::print("br {}", value_string(branch->dst()));
}

void Dumper::visit(CallInst *call) {
    fmt::print("call {}(", value_string(call->callee()));
    for (bool first = true; auto *arg : call->args()) {
        if (!first) {
            fmt::print(", ");
        }
        first = false;
        fmt::print("{}", value_string(arg));
    }
    fmt::print(")");
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

void dump(Unit &unit) {
    for (auto *function : unit) {
        Dumper dumper(function);
        fmt::print("fn @{}(", function->name());
        for (std::size_t i = 0; i < function->arguments().size(); i++) {
            if (i != 0) {
                fmt::print(", ");
            }
            fmt::print("%a{}", i);
        }
        fmt::print(") {{\n");
        for (auto *block : *function) {
            dumper.dump(*block);
        }
        fmt::print("}}\n", function->name());
    }
}

} // namespace ir
