#include <coel/x86/Backend.hh>

#include <coel/graph/DepthFirstSearch.hh>
#include <coel/ir/BasicBlock.hh>
#include <coel/ir/Constant.hh>
#include <coel/ir/Function.hh>
#include <coel/ir/InstVisitor.hh>
#include <coel/ir/Instructions.hh>
#include <coel/ir/Types.hh>
#include <coel/ir/Unit.hh>
#include <coel/support/Assert.hh>
#include <coel/x86/Builder.hh>
#include <coel/x86/Register.hh>

#include <unordered_map>
#include <vector>

namespace coel::x86 {
namespace {

class Compiler final : public ir::InstVisitor {
    const ir::Function *m_function{nullptr};
    std::unordered_map<const ir::StackSlot *, std::int32_t> m_stack_offsets;
    std::vector<MachineInst> m_insts;

    Builder emit(Opcode opcode);
    void emit_rhs(Builder inst, ir::Value *rhs);
    std::uint8_t type_width(const ir::Type *type);

public:
    void run(const ir::Function *function);
    void visit(ir::BinaryInst *) override;
    void visit(ir::BranchInst *) override;
    void visit(ir::CallInst *) override;
    void visit(ir::CompareInst *) override;
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
        COEL_ASSERT(inst.operand_width() == type_width(reg->type()));
        inst.reg(static_cast<Register>(reg->reg()));
    } else if (auto *constant = rhs->as<ir::Constant>()) {
        inst.imm(constant->value()).width(type_width(constant->type()));
    } else if (auto *load = rhs->as<ir::LoadInst>()) {
        const auto *stack_slot = load->ptr()->as_non_null<ir::StackSlot>();
        inst.base_disp(Register::rbp, m_stack_offsets.at(stack_slot));
        inst.width(type_width(load->type()));
    } else {
        COEL_ENSURE_NOT_REACHED();
    }
}

std::uint8_t Compiler::type_width(const ir::Type *type) {
    if (const auto *bool_type = type->as<ir::BoolType>()) {
        return 16;
    }
    if (const auto *integer_type = type->as<ir::IntegerType>()) {
        // TODO: Remove this once 8-bit is properly supported/most of it is legalised away anyway.
        if (integer_type->bit_width() == 8) {
            return 16;
        }
        return integer_type->bit_width();
    }
    COEL_ENSURE_NOT_REACHED();
}

void Compiler::run(const ir::Function *function) {
    m_function = function;
    emit(Opcode::Lbl).lbl(function);
    if (!function->stack_slots().empty()) {
        std::size_t frame_size = 0;
        for (std::int32_t offset = 0; const auto *stack_slot : function->stack_slots()) {
            const std::uint8_t size =
                type_width(stack_slot->type()->as_non_null<ir::PointerType>()->pointee_type()) / 8;
            frame_size += size;
            offset -= size;
            m_stack_offsets.emplace(stack_slot, offset);
        }
        emit(Opcode::Push).reg(Register::rbp).width(64);
        emit(Opcode::Mov).reg(Register::rbp).reg(Register::rsp).width(64);
        emit(Opcode::Sub).reg(Register::rsp).imm(frame_size).width(64);
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
    COEL_ASSERT(lhs->physical());
    auto inst = emit(opcode(binary->op())).reg(static_cast<Register>(lhs->reg())).width(type_width(binary->type()));
    emit_rhs(inst, binary->rhs());
}

void Compiler::visit(ir::BranchInst *branch) {
    emit(Opcode::JmpLbl).lbl(branch->dst());
}

void Compiler::visit(ir::CallInst *call) {
    emit(Opcode::CallLbl).lbl(call->callee());
}

void Compiler::visit(ir::CompareInst *compare) {
    const auto *lhs = compare->lhs()->as_non_null<codegen::Register>();
    COEL_ASSERT(lhs->physical());
    auto cmp = emit(Opcode::Cmp).reg(static_cast<Register>(lhs->reg())).width(type_width(lhs->type()));
    emit_rhs(cmp, compare->rhs());
    auto opcode = [](ir::CompareOp op) -> Opcode {
        switch (op) {
        case ir::CompareOp::Eq:
            return Opcode::Sete;
        case ir::CompareOp::Ne:
            return Opcode::Setne;
        case ir::CompareOp::Lt:
            return Opcode::Setl;
        case ir::CompareOp::Gt:
            return Opcode::Setg;
        case ir::CompareOp::Le:
            return Opcode::Setle;
        case ir::CompareOp::Ge:
            return Opcode::Setge;
        }
    };
    emit(opcode(compare->op())).reg(static_cast<Register>(lhs->reg())).width(8);
}

void Compiler::visit(ir::CondBranchInst *cond_branch) {
    const auto *cond = cond_branch->cond()->as_non_null<codegen::Register>();
    COEL_ASSERT(cond->physical());
    emit(Opcode::Cmp).reg(static_cast<Register>(cond->reg())).imm(1).width(type_width(cond->type()));
    emit(Opcode::JeLbl).lbl(cond_branch->true_dst());
    emit(Opcode::JmpLbl).lbl(cond_branch->false_dst());
}

void Compiler::visit(ir::CopyInst *copy) {
    COEL_ASSERT(copy->dst()->physical());
    auto inst = emit(Opcode::Mov).reg(static_cast<Register>(copy->dst()->reg())).width(type_width(copy->dst()->type()));
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
    auto inst = emit(Opcode::Mov)
                    .base_disp(Register::rbp, m_stack_offsets.at(stack_slot))
                    .width(type_width(stack_slot->type()->as_non_null<ir::PointerType>()->pointee_type()));
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
            inst.operands[0].off = static_cast<std::int64_t>(label_map.at(inst.operands[0].lbl) - ret.size());
            break;
        case Opcode::JeLbl:
            inst.opcode = Opcode::Je;
            inst.operands[0].type = OperandType::Off;
            inst.operands[0].off = static_cast<std::int64_t>(label_map.at(inst.operands[0].lbl) - ret.size());
            break;
        case Opcode::JmpLbl:
            inst.opcode = Opcode::Jmp;
            inst.operands[0].type = OperandType::Off;
            inst.operands[0].off = static_cast<std::int64_t>(label_map.at(inst.operands[0].lbl) - ret.size());
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

} // namespace coel::x86
