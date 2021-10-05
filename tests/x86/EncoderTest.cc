#include <coel/x86/Builder.hh>
#include <coel/x86/MachineInst.hh>

#include <gtest/gtest.h>

#include <array>

namespace coel::x86 {
namespace {

#define BUILD(op)                                                                                                      \
    MachineInst inst{};                                                                                                \
    inst.opcode = (op);                                                                                                \
    Builder(&inst)

#define BUILD_NO_OPERANDS(op)                                                                                          \
    MachineInst inst{};                                                                                                \
    inst.opcode = (op)

std::pair<std::array<std::uint8_t, 16>, std::uint8_t> encode(const MachineInst &inst) {
    std::array<std::uint8_t, 16> encoded{};
    std::uint8_t length = encode(inst, encoded);
    return std::make_pair(encoded, length);
}

TEST(x86EncoderTest, Add64Reg_raxBase_rbpDisp8) {
    BUILD(Opcode::Add).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x03); // add r64, r/m64
    EXPECT_EQ(encoded[2], 0x45); // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Add64Reg_raxBase_r11Disp8) {
    BUILD(Opcode::Add).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x03); // add r64, r/m64
    EXPECT_EQ(encoded[2], 0x43); // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Add64Reg_r10Base_rbpDisp8) {
    BUILD(Opcode::Add).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c); // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], 0x03); // add r64, r/m64
    EXPECT_EQ(encoded[2], 0x55); // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Add64Reg_r10Base_r11Disp8) {
    BUILD(Opcode::Add).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x03); // add r64, r/m64
    EXPECT_EQ(encoded[2], 0x53); // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Add64Reg_rbxImm8) {
    BUILD(Opcode::Add).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x83); // add r/m64, imm8
    EXPECT_EQ(encoded[2], 0xc3); // modrm(0b11, 0, rbx=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Add64Reg_r11Imm8) {
    BUILD(Opcode::Add).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x83); // add r/m64, imm8
    EXPECT_EQ(encoded[2], 0xc3); // modrm(0b11, 0, r11=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Add64Reg_raxReg_rbx) {
    BUILD(Opcode::Add).reg(Register::rax).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x01); // add r/m64, r64
    EXPECT_EQ(encoded[2], 0xd8); // modrm(0b11, rbx=3, rax=0)
}

TEST(x86EncoderTest, Add64Reg_r11Reg_r12) {
    BUILD(Opcode::Add).reg(Register::r11).reg(Register::r12);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r12) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x01); // add r/m64, r64
    EXPECT_EQ(encoded[2], 0xe3); // modrm(0b11, r12=4, r11=3)
}

TEST(x86EncoderTest, CallOff32) {
    BUILD(Opcode::Call).off(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0xe8); // call off32
    EXPECT_EQ(encoded[1], 0xfb);
    EXPECT_EQ(encoded[2], 0xff);
    EXPECT_EQ(encoded[3], 0xff);
    EXPECT_EQ(encoded[4], 0xff);
}

TEST(x86EncoderTest, Cmp64Reg_raxBase_rbpDisp8) {
    BUILD(Opcode::Cmp).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x3b); // cmp r64, r/m64
    EXPECT_EQ(encoded[2], 0x45); // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Cmp64Reg_raxBase_r11Disp8) {
    BUILD(Opcode::Cmp).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x3b); // cmp r64, r/m64
    EXPECT_EQ(encoded[2], 0x43); // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Cmp64Reg_r10Base_rbpDisp8) {
    BUILD(Opcode::Cmp).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c); // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], 0x3b); // cmp r64, r/m64
    EXPECT_EQ(encoded[2], 0x55); // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Cmp64Reg_r10Base_r11Disp8) {
    BUILD(Opcode::Sub).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x2b); // cmp r64, r/m64
    EXPECT_EQ(encoded[2], 0x53); // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Cmp64Reg_rbxImm8) {
    BUILD(Opcode::Cmp).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x83); // cmp r/m64, imm8
    EXPECT_EQ(encoded[2], 0xfb); // modrm(0b11, /7, rbx=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Cmp64Reg_r11Imm8) {
    BUILD(Opcode::Cmp).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x83); // cmp r/m64, imm8
    EXPECT_EQ(encoded[2], 0xfb); // modrm(0b11, /7, r11=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Leave64) {
    BUILD_NO_OPERANDS(Opcode::Leave);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 1);
    EXPECT_EQ(encoded[0], 0xc9); // leave
}

TEST(x86EncoderTest, Mov32Reg_ebxImm32) {
    BUILD(Opcode::Mov).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0xbb); // mov r32, imm32 = b8 + rbx=3
    EXPECT_EQ(encoded[1], 0x00);
    EXPECT_EQ(encoded[2], 0x00);
    EXPECT_EQ(encoded[3], 0x00);
    EXPECT_EQ(encoded[4], 0x00);
}

TEST(x86EncoderTest, Mov32Reg_r11dImm32) {
    BUILD(Opcode::Mov).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 6);
    EXPECT_EQ(encoded[0], 0x41); // REX.B(r11)
    EXPECT_EQ(encoded[1], 0xbb); // mov r32, imm32 = b8 + r11=3
    EXPECT_EQ(encoded[2], 0x00);
    EXPECT_EQ(encoded[3], 0x00);
    EXPECT_EQ(encoded[4], 0x00);
    EXPECT_EQ(encoded[5], 0x00);
}

TEST(x86EncoderTest, Mov64Base_rbpDisp8Reg_rax) {
    BUILD(Opcode::Mov).base_disp(Register::rbp, 0).reg(Register::rax);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x45); // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Base_rbpDisp8Reg_r10) {
    BUILD(Opcode::Mov).base_disp(Register::rbp, 0).reg(Register::r10);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c); // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x55); // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Base_r11Disp8Reg_rax) {
    BUILD(Opcode::Mov).base_disp(Register::r11, 0).reg(Register::rax);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x43); // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Base_r11Disp8Reg_r10) {
    BUILD(Opcode::Mov).base_disp(Register::r11, 0).reg(Register::r10);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x53); // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_raxBase_rbpDisp8) {
    BUILD(Opcode::Mov).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x45); // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_raxBase_r11Disp8) {
    BUILD(Opcode::Mov).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x43); // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_r10Base_rbpDisp8) {
    BUILD(Opcode::Mov).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c); // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x55); // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_r10Base_r11Disp8) {
    BUILD(Opcode::Mov).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x53); // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_raxReg_rbx) {
    BUILD(Opcode::Mov).reg(Register::rax).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0xd8); // modrm(0b11, rbx=3, rax=0)
}

TEST(x86EncoderTest, Mov64Reg_r11Reg_r12) {
    BUILD(Opcode::Mov).reg(Register::r11).reg(Register::r12);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r12) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0xe3); // modrm(0b11, r12=4, r11=3)
}

TEST(x86EncoderTest, PopReg_rbx) {
    BUILD(Opcode::Pop).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 1);
    EXPECT_EQ(encoded[0], 0x5b); // pop r64 = 58 + rbx=3
}

TEST(x86EncoderTest, PopReg_r11) {
    BUILD(Opcode::Pop).reg(Register::r11);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 2);
    EXPECT_EQ(encoded[0], 0x41); // REX.B
    EXPECT_EQ(encoded[1], 0x5b); // pop r64 = 58 + r11=3
}

TEST(x86EncoderTest, PushReg_rbx) {
    BUILD(Opcode::Push).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 1);
    EXPECT_EQ(encoded[0], 0x53); // push r64 = 50 + rbx=3
}

TEST(x86EncoderTest, PushReg_r11) {
    BUILD(Opcode::Push).reg(Register::r11);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 2);
    EXPECT_EQ(encoded[0], 0x41); // REX.B
    EXPECT_EQ(encoded[1], 0x53); // push r64 = 50 + r11=3
}

TEST(x86EncoderTest, Ret) {
    BUILD_NO_OPERANDS(Opcode::Ret);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 1);
    EXPECT_EQ(encoded[0], 0xc3); // ret
}

class Setcc : public testing::TestWithParam<std::pair<Opcode, std::uint8_t>> {};

TEST_P(Setcc, Setcc_al) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode).reg(Register::rax);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x0f);           // secondary opcode table
    EXPECT_EQ(encoded[1], encoded_opcode); // setcc r/m
    EXPECT_EQ(encoded[2], 0xc0);           // modrm(0b11, 0, al=0)
}

TEST_P(Setcc, Setcc_spl) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode).reg(Register::rsp);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x40);           // REX
    EXPECT_EQ(encoded[1], 0x0f);           // secondary opcode table
    EXPECT_EQ(encoded[2], encoded_opcode); // setcc r/m
    EXPECT_EQ(encoded[3], 0xc4);           // modrm(0b11, 0, spl=4)
}

TEST_P(Setcc, Setcc_sil) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode).reg(Register::rsi);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x40);           // REX
    EXPECT_EQ(encoded[1], 0x0f);           // secondary opcode table
    EXPECT_EQ(encoded[2], encoded_opcode); // setcc r/m
    EXPECT_EQ(encoded[3], 0xc6);           // modrm(0b11, 0, sil=6)
}

TEST_P(Setcc, Setcc_r11b) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode).reg(Register::r11);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x41);           // REX.B(r11)
    EXPECT_EQ(encoded[1], 0x0f);           // secondary opcode table
    EXPECT_EQ(encoded[2], encoded_opcode); // setcc r/m
    EXPECT_EQ(encoded[3], 0xc3);           // modrm(0b11, 0, r11b=3)
}

INSTANTIATE_TEST_SUITE_P(x86EncoderTest, Setcc,
                         testing::Values(std::pair<Opcode, std::uint8_t>(Opcode::Sete, 0x94),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Setne, 0x95),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Setl, 0x9c),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Setg, 0x9f),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Setle, 0x9e),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Setge, 0x9d)));

TEST(x86EncoderTest, Sub64Reg_raxBase_rbpDisp8) {
    BUILD(Opcode::Sub).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x2b); // sub r64, r/m64
    EXPECT_EQ(encoded[2], 0x45); // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Sub64Reg_raxBase_r11Disp8) {
    BUILD(Opcode::Sub).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x2b); // sub r64, r/m64
    EXPECT_EQ(encoded[2], 0x43); // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Sub64Reg_r10Base_rbpDisp8) {
    BUILD(Opcode::Sub).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c); // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], 0x2b); // sub r64, r/m64
    EXPECT_EQ(encoded[2], 0x55); // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Sub64Reg_r10Base_r11Disp8) {
    BUILD(Opcode::Sub).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x2b); // sub r64, r/m64
    EXPECT_EQ(encoded[2], 0x53); // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Sub64Reg_rbxImm8) {
    BUILD(Opcode::Sub).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x83); // sub r/m64, imm8
    EXPECT_EQ(encoded[2], 0xeb); // modrm(0b11, /5, rbx=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Sub64Reg_r11Imm8) {
    BUILD(Opcode::Sub).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x83); // sub r/m64, imm8
    EXPECT_EQ(encoded[2], 0xeb); // modrm(0b11, /5, r11=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Sub64Reg_raxReg_rbx) {
    BUILD(Opcode::Sub).reg(Register::rax).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x29); // sub r/m64, r64
    EXPECT_EQ(encoded[2], 0xd8); // modrm(0b11, rbx=3, rax=0)
}

TEST(x86EncoderTest, Sub64Reg_r11Reg_r12) {
    BUILD(Opcode::Sub).reg(Register::r11).reg(Register::r12);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r12) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x29); // sub r/m64, r64
    EXPECT_EQ(encoded[2], 0xe3); // modrm(0b11, r12=4, r11=3)
}

} // namespace
} // namespace coel::x86
