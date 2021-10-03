#pragma once

#include <codegen/x86/Register.hh>

#include <array>
#include <cstdint>
#include <span>

namespace x86 {

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
            Register base;
        };
        std::uint64_t imm;
        const void *lbl;
        std::int64_t off;
        Register reg;
    };
};

struct MachineInst {
    Opcode opcode;
    std::array<Operand, 4> operands;
};

std::uint8_t encode(const MachineInst &inst, std::span<std::uint8_t, 16> encoded);

} // namespace x86
