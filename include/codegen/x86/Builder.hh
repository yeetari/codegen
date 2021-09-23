#pragma once

#include <codegen/x86/Register.hh>

#include <cstdint>

namespace x86 {

class MachineInst;

class Builder {
    MachineInst *const m_inst;
    std::uint8_t m_operand_count{0};

public:
    explicit Builder(MachineInst *inst) : m_inst(inst) {}

    Builder base_disp(Register base, std::int32_t disp);
    Builder imm(std::uint64_t op);
    Builder lbl(const void *op);
    Builder reg(Register op);
};

} // namespace x86
