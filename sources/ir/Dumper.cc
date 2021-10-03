#include <coel/ir/Dumper.hh>

#include <coel/ir/BasicBlock.hh>
#include <coel/ir/Constant.hh>
#include <coel/ir/Function.hh>
#include <coel/ir/InstVisitor.hh>
#include <coel/ir/Instructions.hh>
#include <coel/ir/Unit.hh>
#include <coel/support/Assert.hh>

#include <fmt/core.h>

#include <string>
#include <unordered_map>

namespace coel::ir {
namespace {

class Dumper final : public InstVisitor {
    const Function *const m_function;
    std::unordered_map<BasicBlock *, std::size_t> m_block_map;
    std::unordered_map<StackSlot *, std::size_t> m_stack_map;
    std::unordered_map<Value *, std::size_t> m_value_map;

    std::string value_string(Value *value);

public:
    explicit Dumper(const Function *function) : m_function(function) {}

    void dump(BasicBlock &block);
    void dump_stack_slots();
    void visit(BinaryInst *) override;
    void visit(BranchInst *) override;
    void visit(CallInst *) override;
    void visit(CompareInst *) override;
    void visit(CondBranchInst *) override;
    void visit(CopyInst *) override;
    void visit(LoadInst *) override;
    void visit(RetInst *) override;
    void visit(StoreInst *) override;
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
    if (auto *stack_slot = value->as<StackSlot>()) {
        if (!m_stack_map.contains(stack_slot)) {
            m_stack_map.emplace(stack_slot, m_stack_map.size());
        }
        return fmt::format("%s{}", m_stack_map.at(stack_slot));
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
        if (!inst->is_terminator() && !inst->is<CopyInst>() && !inst->is<StoreInst>()) {
            fmt::print("{} = ", value_string(inst));
        }
        inst->accept(this);
        fmt::print("\n");
    }
    fmt::print("  }}\n");
}

void Dumper::dump_stack_slots() {
    for (auto *stack_slot : m_function->stack_slots()) {
        fmt::print("  {}: i32\n", value_string(stack_slot));
    }
}

void Dumper::visit(BinaryInst *binary) {
    auto op_string = [](BinaryOp op) -> std::string_view {
        using namespace std::literals;
        switch (op) {
        case BinaryOp::Add:
            return "add"sv;
        case BinaryOp::Sub:
            return "sub"sv;
        }
    };
    fmt::print("{} {}, {}", op_string(binary->op()), value_string(binary->lhs()), value_string(binary->rhs()));
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

void Dumper::visit(CompareInst *compare) {
    auto op_string = [](CompareOp op) -> std::string_view {
        using namespace std::literals;
        switch (op) {
        case CompareOp::Eq:
            return "cmp_eq"sv;
        case CompareOp::Ne:
            return "cmp_ne"sv;
        case CompareOp::Lt:
            return "cmp_lt"sv;
        case CompareOp::Gt:
            return "cmp_gt"sv;
        case CompareOp::Le:
            return "cmp_le"sv;
        case CompareOp::Ge:
            return "cmp_ge"sv;
        }
    };
    fmt::print("{} {}, {}", op_string(compare->op()), value_string(compare->lhs()), value_string(compare->rhs()));
}

void Dumper::visit(CondBranchInst *cond_branch) {
    fmt::print("br {}, {}, {}", value_string(cond_branch->cond()), value_string(cond_branch->true_dst()),
               value_string(cond_branch->false_dst()));
}

void Dumper::visit(CopyInst *copy) {
    fmt::print("copy {}, {}", value_string(copy->dst()), value_string(copy->src()));
}

void Dumper::visit(LoadInst *load) {
    fmt::print("load {}", value_string(load->ptr()));
}

void Dumper::visit(RetInst *ret) {
    fmt::print("ret {}", value_string(ret->value()));
}

void Dumper::visit(StoreInst *store) {
    fmt::print("store {}, {}", value_string(store->ptr()), value_string(store->value()));
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
        dumper.dump_stack_slots();
        for (auto *block : *function) {
            dumper.dump(*block);
        }
        fmt::print("}}\n", function->name());
    }
}

} // namespace coel::ir
