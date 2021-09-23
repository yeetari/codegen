#include <x86/MachineInst.hh>

#include <support/Assert.hh>

namespace x86 {
namespace {

std::uint8_t emit_mod_rm(std::uint8_t mod, std::uint8_t reg, std::uint8_t rm) {
    return ((mod & 0b11u) << 6u) | ((reg & 0b111u) << 3u) | (rm & 0b111u);
}

} // namespace

std::uint8_t encode(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    std::uint8_t length = 0;
    switch (inst.opcode) {
    case Opcode::Add: {
        if (inst.operands[0].type == OperandType::Reg && inst.operands[1].type == OperandType::Reg) {
            auto lhs = static_cast<std::uint8_t>(inst.operands[0].reg);
            auto rhs = static_cast<std::uint8_t>(inst.operands[1].reg);
            encoded[length++] = 0x48; // REX.W
            encoded[length++] = 0x01; // ADD r/m64, r64
            encoded[length++] = emit_mod_rm(0b11, rhs, lhs);
            break;
        }
        ASSERT(inst.operands[0].type == OperandType::Reg && inst.operands[1].type == OperandType::Imm);
        auto dst = static_cast<std::uint8_t>(inst.operands[0].reg);
        auto imm = static_cast<std::uint8_t>(inst.operands[1].imm & 0xffu);
        ASSERT(imm <= 0x7f);
        if (dst >= 8) {
            encoded[length++] = 0x49; // REX.W + REX.B
        } else {
            encoded[length++] = 0x48; // REX.W
        }
        encoded[length++] = 0x83; // add r/m64, imm8
        encoded[length++] = emit_mod_rm(0b11, 0, dst);
        encoded[length++] = imm;
        break;
    }
    case Opcode::Call: {
        ASSERT(inst.operands[0].type == OperandType::Off);
        auto off = (static_cast<std::uint64_t>(inst.operands[0].off) & 0xffffffffu) - 5;
        encoded[length++] = 0xe8;
        encoded[length++] = (off >> 0u) & 0xffu;
        encoded[length++] = (off >> 8u) & 0xffu;
        encoded[length++] = (off >> 16u) & 0xffu;
        encoded[length++] = (off >> 24u) & 0xffu;
        break;
    }
    case Opcode::Cmp: {
        ASSERT(inst.operands[0].type == OperandType::Reg && inst.operands[1].type == OperandType::Imm);
        auto dst = static_cast<std::uint8_t>(inst.operands[0].reg);
        auto imm = static_cast<std::uint8_t>(inst.operands[1].imm & 0xffu);
        ASSERT(imm <= 0x7f);
        encoded[length++] = 0x48; // REX.W
        encoded[length++] = 0x83; // cmp r/m64, imm8
        encoded[length++] = emit_mod_rm(0b11, 7, dst);
        encoded[length++] = imm;
        break;
    }
    case Opcode::Je: {
        ASSERT(inst.operands[0].type == OperandType::Off);
        encoded[length++] = 0x74;
        encoded[length++] = inst.operands[0].off - 2;
        break;
    }
    case Opcode::Jmp: {
        ASSERT(inst.operands[0].type == OperandType::Off);
        encoded[length++] = 0xeb;
        encoded[length++] = inst.operands[0].off - 2;
        break;
    }
    case Opcode::Mov: {
        if (inst.operands[0].type == OperandType::Reg && inst.operands[1].type == OperandType::Imm) {
            auto lhs = static_cast<std::uint8_t>(inst.operands[0].reg);
            auto rhs = static_cast<std::uint32_t>(inst.operands[1].imm & 0xffffffffu);
            encoded[length++] = 0xb8 + lhs;
            encoded[length++] = (rhs >> 0u) & 0xffu;
            encoded[length++] = (rhs >> 8u) & 0xffu;
            encoded[length++] = (rhs >> 16u) & 0xffu;
            encoded[length++] = (rhs >> 24u) & 0xffu;
            break;
        }
        ASSERT(inst.operands[0].type == OperandType::Reg && inst.operands[1].type == OperandType::Reg);
        auto dst = static_cast<std::uint8_t>(inst.operands[0].reg);
        auto src = static_cast<std::uint8_t>(inst.operands[1].reg);
        encoded[length++] = 0x48; // REX.W
        encoded[length++] = 0x89; // mov r/m64, r64
        encoded[length++] = emit_mod_rm(0b11, src, dst);
        break;
    }
    case Opcode::Pop: {
        ASSERT(inst.operands[0].type == OperandType::Reg);
        auto reg = static_cast<std::uint8_t>(inst.operands[0].reg);
        if (reg >= 8) {
            encoded[length++] = 0x41; // REX.B
            reg -= 8;
        }
        encoded[length++] = 0x58 + reg; // pop r64
        break;
    }
    case Opcode::Push: {
        ASSERT(inst.operands[0].type == OperandType::Reg);
        auto reg = static_cast<std::uint8_t>(inst.operands[0].reg);
        if (reg >= 8) {
            encoded[length++] = 0x41; // REX.B
            reg -= 8;
        }
        encoded[length++] = 0x50 + reg; // push r64
        break;
    }
    case Opcode::Ret:
        ASSERT(inst.operands[0].type == OperandType::None);
        encoded[length++] = 0xc3; // ret
        break;
    default:
        ENSURE_NOT_REACHED();
    }
    return length;
}

} // namespace x86
