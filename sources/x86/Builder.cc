#include <codegen/x86/Builder.hh>

#include <codegen/x86/MachineInst.hh>
#include <codegen/x86/Register.hh>

#include <cstdint>

namespace x86 {

Builder Builder::base_disp(Register, std::int32_t) {
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

} // namespace x86
