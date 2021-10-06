#include <coel/x86/Builder.hh>

#include <coel/x86/MachineInst.hh>
#include <coel/x86/Register.hh>

#include <cstdint>

namespace coel::x86 {

Builder Builder::base_disp(Register base, std::int32_t disp) {
    m_inst->operands[m_operand_count].type = OperandType::BaseDisp;
    m_inst->operands[m_operand_count].base = base;
    m_inst->operands[m_operand_count++].disp = disp;
    return *this;
}

Builder Builder::imm(std::uint64_t op) {
    m_inst->operands[m_operand_count].type = OperandType::Imm;
    m_inst->operands[m_operand_count++].imm = op;
    return *this;
}

Builder Builder::lbl(const void *op) {
    m_inst->operands[m_operand_count].type = OperandType::Lbl;
    m_inst->operands[m_operand_count++].lbl = op;
    return *this;
}

Builder Builder::off(std::int64_t op) {
    m_inst->operands[m_operand_count].type = OperandType::Off;
    m_inst->operands[m_operand_count++].off = op;
    return *this;
}

Builder Builder::reg(Register op) {
    m_inst->operands[m_operand_count].type = OperandType::Reg;
    m_inst->operands[m_operand_count++].reg = op;
    return *this;
}

Builder Builder::width(std::uint8_t operand_width) {
    m_inst->operand_width = operand_width;
    return *this;
}

std::uint8_t Builder::operand_width() const {
    return m_inst->operand_width;
}

} // namespace coel::x86
