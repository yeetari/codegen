#include <coel/x86/MachineInst.hh>

#include <coel/support/Assert.hh>

#include <array>
#include <limits>

namespace coel::x86 {
namespace {

std::uint8_t emit_mod_rm(std::uint8_t mod, std::uint8_t reg, std::uint8_t rm) {
    return ((mod & 0b11u) << 6u) | ((reg & 0b111u) << 3u) | (rm & 0b111u);
}

std::uint8_t encode_arith(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    COEL_ASSERT(inst.operands[0].type == OperandType::Reg);
    auto lhs = static_cast<std::uint8_t>(inst.operands[0].reg);
    std::uint8_t rex = 0x48; // REX.W
    if (lhs >= 8) {
        rex |= (1u << 0u); // REX.B
    }
    switch (inst.operands[1].type) {
    case OperandType::BaseDisp: {
        rex = 0x48;
        if (lhs >= 8) {
            rex |= (1u << 2u); // REX.R
        }
        auto base = static_cast<std::uint8_t>(inst.operands[1].base);
        if (base >= 8) {
            rex |= (1u << 0u); // REX.B
        }
        encoded[0] = rex;
        if (inst.opcode == Opcode::Add) {
            encoded[1] = 0x03; // add reg, r/m
        } else {
            encoded[1] = 0x2b; // sub reg, r/m
        }
        encoded[2] = emit_mod_rm(0b01, lhs, base);
        encoded[3] = inst.operands[1].disp;
        return 4;
    }
    case OperandType::Imm: {
        // TODO: Emit special encoding for add/sub (al, ax, eax, rax), imm(8, 16, 32, 32).
        auto rhs = static_cast<std::uint8_t>(inst.operands[1].imm & 0xffu);
        COEL_ASSERT(rhs <= 0x7f);
        encoded[0] = rex;
        encoded[1] = 0x83; // add/sub r/m, imm8
        encoded[2] = emit_mod_rm(0b11, inst.opcode == Opcode::Sub ? 5 : 0, lhs);
        encoded[3] = rhs;
        return 4;
    }
    case OperandType::Reg: {
        auto rhs = static_cast<std::uint8_t>(inst.operands[1].reg);
        if (rhs >= 8) {
            rex |= (1u << 2u); // REX.R
        }
        encoded[0] = rex;
        if (inst.opcode == Opcode::Add) {
            encoded[1] = 0x01; // add r/m, reg
        } else {
            encoded[1] = 0x29; // sub r/m, reg
        }
        encoded[2] = emit_mod_rm(0b11, rhs, lhs);
        return 3;
    }
    default:
        COEL_ENSURE_NOT_REACHED();
    }
}

std::uint8_t encode_cmp(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    // TODO: Emit special encoding for cmp (al, ax, eax, rax), imm(8, 16, 32, 64).
    COEL_ASSERT(inst.operands[0].type == OperandType::Reg);
    auto lhs = static_cast<std::uint8_t>(inst.operands[0].reg);
    if (lhs >= 8) {
        encoded[0] = 0x49; // REX.W + REX.B
    } else {
        encoded[0] = 0x48; // REX.W
    }
    switch (inst.operands[1].type) {
    case OperandType::BaseDisp: {
        auto base = static_cast<std::uint8_t>(inst.operands[1].base);
        std::uint8_t rex = 0x48;
        if (lhs >= 8) {
            rex |= (1u << 2u); // REX.R
        }
        if (base >= 8) {
            rex |= (1u << 0u); // REX.B
        }
        encoded[0] = rex;
        encoded[1] = 0x3b; // cmp reg, r/m
        encoded[2] = emit_mod_rm(0b01, lhs, base);
        encoded[3] = inst.operands[1].disp;
        return 4;
    }
    case OperandType::Imm: {
        auto rhs = static_cast<std::uint8_t>(inst.operands[1].imm & 0xffu);
        COEL_ASSERT(rhs <= 0x7f);
        encoded[1] = 0x83; // cmp r/m, imm8
        encoded[2] = emit_mod_rm(0b11, 7, lhs);
        encoded[3] = rhs;
        return 4;
    }
    case OperandType::Reg: {
        auto rhs = static_cast<std::uint8_t>(inst.operands[1].reg);
        encoded[1] = 0x39; // cmp r/m, reg
        encoded[2] = emit_mod_rm(0b11, rhs, lhs);
        return 3;
    }
    default:
        COEL_ENSURE_NOT_REACHED();
    }
}

std::uint8_t encode_leave(const MachineInst &, std::span<std::uint8_t, 16> encoded) {
    encoded[0] = 0xc9;
    return 1;
}

std::uint8_t encode_mov(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    std::uint8_t mod = 0;
    std::uint8_t dst = 0;
    switch (inst.operands[0].type) {
    case OperandType::BaseDisp: {
        mod = 0b01;
        dst = static_cast<std::uint8_t>(inst.operands[0].base);
        break;
    }
    case OperandType::Reg: {
        mod = 0b11;
        dst = static_cast<std::uint8_t>(inst.operands[0].reg);
        break;
    }
    default:
        COEL_ENSURE_NOT_REACHED();
    }

    std::uint8_t length = 0;
    std::uint8_t rex = 0x40;
    if (dst >= 8) {
        rex |= (1u << 0u); // REX.B
    }
    switch (inst.operands[1].type) {
    case OperandType::BaseDisp: {
        // TODO: Use 32-bit displacement if needed.
        COEL_ASSERT(inst.operands[1].disp > std::numeric_limits<std::int8_t>::min() &&
               inst.operands[1].disp < std::numeric_limits<std::int8_t>::max());
        bool need_sib = inst.operands[1].base == Register::rsp || inst.operands[1].base == Register::r12;
        COEL_ASSERT(!need_sib); // TODO: Support rsp and r12.
        if (rex != 0x40) {
            rex = 0x40u | (1u << 2u);
        }
        auto base = static_cast<std::uint8_t>(inst.operands[1].base);
        if (base >= 8) {
            rex |= (1u << 0u); // REX.B
        }
        rex |= (1u << 3u); // REX.W
        encoded[0] = rex;
        encoded[1] = 0x8b; // mov reg, r/m
        encoded[2] = emit_mod_rm(0b01, dst, base);
        encoded[3] = inst.operands[1].disp;
        return 4;
    }
    case OperandType::Imm: {
        auto imm = static_cast<std::uint32_t>(inst.operands[1].imm & 0xffffffffu);
        if (dst >= 8) {
            dst -= 8;
        }
        if (rex != 0x40) {
            encoded[length++] = rex;
        }
        encoded[length++] = 0xb8 + dst;
        encoded[length++] = (imm >> 0u) & 0xffu;
        encoded[length++] = (imm >> 8u) & 0xffu;
        encoded[length++] = (imm >> 16u) & 0xffu;
        encoded[length++] = (imm >> 24u) & 0xffu;
        break;
    }
    case OperandType::Reg: {
        auto src = static_cast<std::uint8_t>(inst.operands[1].reg);
        if (src >= 8) {
            rex |= (1u << 2u); // REX.R
        }
        rex |= (1u << 3u); // REX.W
        encoded[length++] = rex;
        encoded[length++] = 0x89; // mov r/m, reg
        encoded[length++] = emit_mod_rm(mod, src, dst);
        break;
    }
    default:
        COEL_ENSURE_NOT_REACHED();
    }

    if (inst.operands[0].type == OperandType::BaseDisp) {
        encoded[length++] = inst.operands[0].disp;
    }
    return length;
}

std::uint8_t encode_pop(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    COEL_ASSERT(inst.operands[0].type == OperandType::Reg);
    std::uint8_t length = 0;
    auto reg = static_cast<std::uint8_t>(inst.operands[0].reg);
    if (reg >= 8) {
        encoded[length++] = 0x41; // REX.B
        reg -= 8;
    }
    encoded[length++] = 0x58 + reg; // pop reg
    return length;
}

std::uint8_t encode_push(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    COEL_ASSERT(inst.operands[0].type == OperandType::Reg);
    std::uint8_t length = 0;
    auto reg = static_cast<std::uint8_t>(inst.operands[0].reg);
    if (reg >= 8) {
        encoded[length++] = 0x41; // REX.B
        reg -= 8;
    }
    encoded[length++] = 0x50 + reg; // push reg
    return length;
}

std::uint8_t encode_ret(const MachineInst &, std::span<std::uint8_t, 16> encoded) {
    encoded[0] = 0xc3; // ret
    return 1;
}

std::uint8_t encode_call(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    COEL_ASSERT(inst.operands[0].type == OperandType::Off);
    auto off = (static_cast<std::uint64_t>(inst.operands[0].off) & 0xffffffffu) - 5;
    encoded[0] = 0xe8; // call off32
    encoded[1] = (off >> 0u) & 0xffu;
    encoded[2] = (off >> 8u) & 0xffu;
    encoded[3] = (off >> 16u) & 0xffu;
    encoded[4] = (off >> 24u) & 0xffu;
    return 5;
}

std::uint8_t encode_je(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    COEL_ASSERT(inst.operands[0].type == OperandType::Off);
    encoded[0] = 0x74;
    encoded[1] = inst.operands[0].off - 2;
    return 2;
}

std::uint8_t encode_jmp(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    COEL_ASSERT(inst.operands[0].type == OperandType::Off);
    encoded[0] = 0xeb;
    encoded[1] = inst.operands[0].off - 2;
    return 2;
}

std::uint8_t encode_setcc(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    COEL_ASSERT(inst.operands[0].type == OperandType::Reg);
    auto reg = static_cast<std::uint8_t>(inst.operands[0].reg);
    std::uint8_t length = 0;
    if (reg >= 4) {
        encoded[length++] = reg >= 8 ? 0x41 : 0x40;
    }
    if (reg >= 8) {
        reg -= 8;
    }
    encoded[length++] = 0x0f;
    switch (inst.opcode) {
    case Opcode::Sete:
        encoded[length++] = 0x94;
        break;
    case Opcode::Setne:
        encoded[length++] = 0x95;
        break;
    case Opcode::Setl:
        encoded[length++] = 0x9c;
        break;
    case Opcode::Setg:
        encoded[length++] = 0x9f;
        break;
    case Opcode::Setle:
        encoded[length++] = 0x9e;
        break;
    case Opcode::Setge:
        encoded[length++] = 0x9d;
        break;
    default:
        COEL_ENSURE_NOT_REACHED();
    }
    encoded[length++] = emit_mod_rm(0b11, 0, reg);
    return length;
}

const std::array s_functions{
    &encode_arith,
    &encode_cmp,
    &encode_leave,
    &encode_mov,
    &encode_pop,
    &encode_push,
    &encode_ret,
    &encode_arith,
    &encode_call,
    &encode_je,
    &encode_jmp,
    &encode_setcc,
    &encode_setcc,
    &encode_setcc,
    &encode_setcc,
    &encode_setcc,
    &encode_setcc,
};

} // namespace

std::uint8_t encode(const MachineInst &inst, std::span<std::uint8_t, 16> encoded) {
    auto opcode = static_cast<std::size_t>(inst.opcode);
    COEL_ASSERT(opcode < s_functions.size());
    return s_functions[opcode](inst, encoded);
}

} // namespace coel::x86
