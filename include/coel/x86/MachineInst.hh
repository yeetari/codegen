#pragma once

#include <coel/x86/Register.hh>

#include <array>
#include <cstdint>
#include <span>

namespace coel::x86 {

enum class Opcode {
    Add,
    Cmp,
    Leave,
    Mov,
    Pop,
    Push,
    Ret,
    Sub,

    Call,
    Je,
    Jmp,
    Jne,
    Sete,
    Setne,
    Setl,
    Setg,
    Setle,
    Setge,

    Lbl,
    CallLbl,
    JeLbl,
    JmpLbl,
    JneLbl,
};

enum class OperandType {
    None,
    BaseDisp,
    Imm,
    Lbl,
    Off,
    Reg,
};

struct Operand {
    OperandType type;
    union {
        struct {
            std::int32_t disp;
            std::uint8_t base;
        };
        std::uint64_t imm;
        const void *lbl;
        std::int64_t off;
        std::uint8_t reg;
    };
};

struct MachineInst {
    Opcode opcode;
    std::array<Operand, 4> operands;
    std::uint8_t operand_width;
};

std::uint8_t encode(const MachineInst &inst, std::span<std::uint8_t, 16> encoded);

} // namespace coel::x86
