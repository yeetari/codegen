#pragma once

#include <x86/Register.hh>

#include <array>
#include <cstdint>
#include <span>

namespace x86 {

enum class Opcode {
    Add,
    Cmp,
    Mov,
    Pop,
    Push,
    Ret,

    Call,
    Je,
    Jmp,

    Lbl,
    CallLbl,
    JeLbl,
    JmpLbl,
};

enum class OperandType {
    None,
    Imm,
    Lbl,
    Off,
    Reg,
};

struct Operand {
    OperandType type;
    union {
        std::uint64_t imm;
        const void *lbl;
        std::int8_t off;
        Register reg;
    };
};

struct MachineInst {
    Opcode opcode;
    std::array<Operand, 4> operands;
};

std::uint8_t encode(const MachineInst &inst, std::span<std::uint8_t, 16> encoded);

} // namespace x86
