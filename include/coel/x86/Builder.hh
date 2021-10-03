#pragma once

#include <coel/x86/Register.hh>

#include <cstdint>

namespace coel::x86 {

class MachineInst;

class Builder {
    MachineInst *const m_inst;
    std::uint8_t m_operand_count{0};

public:
    explicit Builder(MachineInst *inst) : m_inst(inst) {}

    Builder base_disp(Register base, std::int32_t disp);
    Builder imm(std::uint64_t op);
    Builder lbl(const void *op);
    Builder off(std::int64_t op);
    Builder reg(Register op);
};

} // namespace coel::x86
