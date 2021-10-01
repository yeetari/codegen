#include <codegen/x86/Backend.hh>

#include <codegen/graph/DepthFirstSearch.hh>
#include <codegen/ir/BasicBlock.hh>
#include <codegen/ir/Constant.hh>
#include <codegen/ir/Function.hh>
#include <codegen/ir/InstVisitor.hh>
#include <codegen/ir/Instructions.hh>
#include <codegen/ir/Unit.hh>
#include <codegen/support/Assert.hh>
#include <codegen/x86/Builder.hh>
#include <codegen/x86/Register.hh>

#include <unordered_map>
#include <vector>

namespace x86 {
namespace {

class Compiler final : public ir::InstVisitor {
    const ir::Function *m_function{nullptr};
    std::unordered_map<const ir::StackSlot *, std::int32_t> m_stack_offsets;
    std::vector<MachineInst> m_insts;

    Builder emit(Opcode opcode);
    void emit_rhs(Builder inst, ir::Value *rhs);

public:
    void run(const ir::Function *function);
    void visit(ir::BinaryInst *) override;
    void visit(ir::BranchInst *) override;
    void visit(ir::CallInst *) override;
    void visit(ir::CondBranchInst *) override;
    void visit(ir::CopyInst *) override;
    void visit(ir::LoadInst *) override {}
    void visit(ir::RetInst *) override;
    void visit(ir::StoreInst *) override;

    std::vector<MachineInst> &insts() { return m_insts; }
};

Builder Compiler::emit(Opcode opcode) {
    auto &inst = m_insts.emplace_back();
    inst.opcode = opcode;
    return Builder(&inst);
}

void Compiler::emit_rhs(Builder inst, ir::Value *rhs) {
    if (auto *reg = rhs->as<codegen::Register>()) {
        inst.reg(static_cast<Register>(reg->reg()));
    } else if (auto *constant = rhs->as<ir::Constant>()) {
        inst.imm(constant->value());
    } else if (auto *load = rhs->as<ir::LoadInst>()) {
        const auto *stack_slot = load->ptr()->as_non_null<ir::StackSlot>();
        inst.base_disp(Register::rbp, m_stack_offsets.at(stack_slot));
    }
}

void Compiler::run(const ir::Function *function) {
    m_function = function;
    emit(Opcode::Lbl).lbl(function);
    if (!function->stack_slots().empty()) {
        for (std::int32_t offset = 0; const auto *stack_slot : function->stack_slots()) {
            offset -= 8;
            m_stack_offsets.emplace(stack_slot, offset);
        }
        emit(Opcode::Push).reg(Register::rbp);
        emit(Opcode::Mov).reg(Register::rbp).reg(Register::rsp);
        emit(Opcode::Sub).reg(Register::rsp).imm(function->stack_slots().size() * 8);
    }
    for (auto *block : *function) {
        emit(Opcode::Lbl).lbl(block);
        for (auto *inst : *block) {
            inst->accept(this);
        }
    }
}

void Compiler::visit(ir::BinaryInst *binary) {
    auto opcode = [](ir::BinaryOp op) {
        switch (op) {
        case ir::BinaryOp::Add:
            return Opcode::Add;
        case ir::BinaryOp::Sub:
            return Opcode::Sub;
        }
    };
    const auto *lhs = binary->lhs()->as_non_null<codegen::Register>();
    ASSERT(lhs->physical());
    auto inst = emit(opcode(binary->op())).reg(static_cast<Register>(lhs->reg()));
    emit_rhs(inst, binary->rhs());
}

void Compiler::visit(ir::BranchInst *branch) {
    emit(Opcode::JmpLbl).lbl(branch->dst());
}

void Compiler::visit(ir::CallInst *call) {
    emit(Opcode::CallLbl).lbl(call->callee());
}

void Compiler::visit(ir::CondBranchInst *cond_branch) {
    const auto *cond = cond_branch->cond()->as_non_null<codegen::Register>();
    ASSERT(cond->physical());
    emit(Opcode::Cmp).reg(static_cast<Register>(cond->reg())).imm(1);
    emit(Opcode::JeLbl).lbl(cond_branch->true_dst());
    emit(Opcode::JmpLbl).lbl(cond_branch->false_dst());
}

void Compiler::visit(ir::CopyInst *copy) {
    ASSERT(copy->dst()->physical());
    auto inst = emit(Opcode::Mov).reg(static_cast<Register>(copy->dst()->reg()));
    emit_rhs(inst, copy->src());
}

void Compiler::visit(ir::RetInst *) {
    if (!m_function->stack_slots().empty()) {
        emit(Opcode::Leave);
    }
    emit(Opcode::Ret);
}

void Compiler::visit(ir::StoreInst *store) {
    const auto *stack_slot = store->ptr()->as_non_null<ir::StackSlot>();
    auto inst = emit(Opcode::Mov).base_disp(Register::rbp, m_stack_offsets.at(stack_slot));
    emit_rhs(inst, store->value());
}

} // namespace

std::vector<MachineInst> compile(const ir::Unit &unit) {
    Compiler compiler;
    for (const auto *function : unit) {
        compiler.run(function);
    }
    return std::move(compiler.insts());
}

std::pair<std::size_t, std::vector<std::uint8_t>> encode(const std::vector<MachineInst> &insts,
                                                         const ir::Function *entry) {
    std::unordered_map<const void *, std::size_t> label_map;
    for (std::size_t length = 0; auto inst : insts) {
        switch (inst.opcode) {
        case Opcode::Lbl:
            label_map.emplace(inst.operands[0].lbl, length);
            continue;
        case Opcode::CallLbl:
            inst.opcode = Opcode::Call;
            inst.operands[0].type = OperandType::Off;
            inst.operands[0].off = 0;
            break;
        case Opcode::JeLbl:
        case Opcode::JmpLbl:
            inst.opcode = Opcode::Jmp;
            inst.operands[0].type = OperandType::Off;
            inst.operands[0].off = 0;
            break;
        default:
            break;
        }
        std::array<std::uint8_t, 16> encoded{};
        length += encode(inst, encoded);
    }

    std::vector<std::uint8_t> ret;
    for (auto inst : insts) {
        switch (inst.opcode) {
        case Opcode::Lbl:
            continue;
        case Opcode::CallLbl:
            inst.opcode = Opcode::Call;
            inst.operands[0].type = OperandType::Off;
            inst.operands[0].off = label_map.at(inst.operands[0].lbl) - ret.size();
            break;
        case Opcode::JeLbl:
            inst.opcode = Opcode::Je;
            inst.operands[0].type = OperandType::Off;
            inst.operands[0].off = label_map.at(inst.operands[0].lbl) - ret.size();
            break;
        case Opcode::JmpLbl:
            inst.opcode = Opcode::Jmp;
            inst.operands[0].type = OperandType::Off;
            inst.operands[0].off = label_map.at(inst.operands[0].lbl) - ret.size();
            break;
        default:
            break;
        }
        std::array<std::uint8_t, 16> encoded{};
        const auto length = encode(inst, encoded);
        ret.resize(ret.size() + length);
        std::copy_n(encoded.begin(), length, ret.end() - length);
    }
    return std::make_pair(label_map.at(entry), std::move(ret));
}

} // namespace x86
