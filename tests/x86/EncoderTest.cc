#include <coel/x86/Builder.hh>
#include <coel/x86/MachineInst.hh>

#include <gtest/gtest.h>

#include <array>

namespace coel::x86 {
namespace {

#define BUILD(op, width)                                                                                               \
    MachineInst inst{};                                                                                                \
    inst.opcode = (op);                                                                                                \
    inst.operand_width = (width);                                                                                      \
    Builder(&inst)

#define BUILD_NO_OPERANDS(op)                                                                                          \
    MachineInst inst{};                                                                                                \
    inst.opcode = (op)

std::pair<std::array<std::uint8_t, 16>, std::uint8_t> encode(const MachineInst &inst) {
    std::array<std::uint8_t, 16> encoded{};
    std::uint8_t length = encode(inst, encoded);
    return std::make_pair(encoded, length);
}

class ArithRegImm : public testing::TestWithParam<std::pair<Opcode, std::uint8_t>> {};
class ArithRegReg : public testing::TestWithParam<std::pair<Opcode, std::uint8_t>> {};
class ArithRegRm : public testing::TestWithParam<std::pair<Opcode, std::uint8_t>> {};

TEST_P(ArithRegRm, Arith16Reg_axBase_rbpDisp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 16).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x66);           // operand size override
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r16, r/m64
    EXPECT_EQ(encoded[2], 0x45);           // modrm(0b01, ax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegRm, Arith16Reg_axBase_r11Disp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 16).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0x66);           // operand size override
    EXPECT_EQ(encoded[1], 0x41);           // REX.B(r11)
    EXPECT_EQ(encoded[2], encoded_opcode); // opcode r16, r/m64
    EXPECT_EQ(encoded[3], 0x43);           // modrm(0b01, ax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[4], 0x00);
}

TEST_P(ArithRegRm, Arith16Reg_r10wBase_rbpDisp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 16).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0x66);           // operand size override
    EXPECT_EQ(encoded[1], 0x44);           // REX.R(r10w)
    EXPECT_EQ(encoded[2], encoded_opcode); // opcode r16, r/m64
    EXPECT_EQ(encoded[3], 0x55);           // modrm(0b01, r10w=2, [rbp]+disp8)
    EXPECT_EQ(encoded[4], 0x00);
}

TEST_P(ArithRegRm, Arith16Reg_r10wBase_r11Disp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 16).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0x66);           // operand size override
    EXPECT_EQ(encoded[1], 0x45);           // REX.R(r10w) + REX.B(r11)
    EXPECT_EQ(encoded[2], encoded_opcode); // opcode r16, r/m64
    EXPECT_EQ(encoded[3], 0x53);           // modrm(0b01, r10w=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[4], 0x00);
}

TEST_P(ArithRegImm, Arith16Reg_bxImm8) {
    auto [opcode, slash] = GetParam();
    BUILD(opcode, 16).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x66);                 // operand size override
    EXPECT_EQ(encoded[1], 0x83);                 // opcode r16, imm8
    EXPECT_EQ(encoded[2], 0xc3 | (slash << 3u)); // modrm(0b11, slash, bx=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegImm, Arith16Reg_r11wImm8) {
    auto [opcode, slash] = GetParam();
    BUILD(opcode, 16).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0x66);                 // operand size override
    EXPECT_EQ(encoded[1], 0x41);                 // REX.B(r11w)
    EXPECT_EQ(encoded[2], 0x83);                 // opcode r16, imm8
    EXPECT_EQ(encoded[3], 0xc3 | (slash << 3u)); // modrm(0b11, slash, r11w=3)
    EXPECT_EQ(encoded[4], 0x00);
}

TEST_P(ArithRegReg, Arith16Reg_axReg_bx) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 16).reg(Register::rax).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x66);           // operand size override
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r16, r16
    EXPECT_EQ(encoded[2], 0xd8);           // modrm(0b11, bx=3, ax=0)
}

TEST_P(ArithRegReg, Arith16Reg_r11wReg_r12w) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 16).reg(Register::r11).reg(Register::r12);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x66);           // operand size override
    EXPECT_EQ(encoded[1], 0x45);           // REX.R(r12w) + REX.B(r11w)
    EXPECT_EQ(encoded[2], encoded_opcode); // opcode r16, r16
    EXPECT_EQ(encoded[3], 0xe3);           // modrm(0b11, r12w=4, r11w=3)
}

TEST_P(ArithRegRm, Arith32Reg_eaxBase_rbpDisp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 32).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], encoded_opcode); // opcode r32, r/m64
    EXPECT_EQ(encoded[1], 0x45);           // modrm(0b01, eax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[2], 0x00);
}

TEST_P(ArithRegRm, Arith32Reg_eaxBase_r11Disp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 32).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x41);           // REX.B(r11)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r32, r/m64
    EXPECT_EQ(encoded[2], 0x43);           // modrm(0b01, eax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegRm, Arith32Reg_r10dBase_rbpDisp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 32).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x44);           // REX.R(r10d)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r32, r/m64
    EXPECT_EQ(encoded[2], 0x55);           // modrm(0b01, r10d=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegRm, Arith32Reg_r10dBase_r11Disp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 32).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x45);           // REX.R(r10d) + REX.B(r11)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r32, r/m64
    EXPECT_EQ(encoded[2], 0x53);           // modrm(0b01, r10d=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegImm, Arith32Reg_ebxImm8) {
    auto [opcode, slash] = GetParam();
    BUILD(opcode, 32).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x83);                 // opcode r32, imm8
    EXPECT_EQ(encoded[1], 0xc3 | (slash << 3u)); // modrm(0b11, slash, ebx=3)
    EXPECT_EQ(encoded[2], 0x00);
}

TEST_P(ArithRegImm, Arith32Reg_r11dImm8) {
    auto [opcode, slash] = GetParam();
    BUILD(opcode, 32).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x41);                 // REX.B(r11d)
    EXPECT_EQ(encoded[1], 0x83);                 // opcode r32, imm8
    EXPECT_EQ(encoded[2], 0xc3 | (slash << 3u)); // modrm(0b11, slash, r11d=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegReg, Arith32Reg_eaxReg_ebx) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 32).reg(Register::rax).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 2);
    EXPECT_EQ(encoded[0], encoded_opcode); // opcode r32, r32
    EXPECT_EQ(encoded[1], 0xd8);           // modrm(0b11, ebx=3, eax=0)
}

TEST_P(ArithRegReg, Arith32Reg_r11dReg_r12d) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 32).reg(Register::r11).reg(Register::r12);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x45);           // REX.R(r12d) + REX.B(r11d)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r32, r32
    EXPECT_EQ(encoded[2], 0xe3);           // modrm(0b11, r12d=4, r11d=3)
}

TEST_P(ArithRegRm, Arith64Reg_raxBase_rbpDisp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 64).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48);           // REX.W
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r64, r/m64
    EXPECT_EQ(encoded[2], 0x45);           // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegRm, Arith64Reg_raxBase_r11Disp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 64).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49);           // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r64, r/m64
    EXPECT_EQ(encoded[2], 0x43);           // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegRm, Arith64Reg_r10Base_rbpDisp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 64).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c);           // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r64, r/m64
    EXPECT_EQ(encoded[2], 0x55);           // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegRm, Arith64Reg_r10Base_r11Disp8) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 64).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d);           // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r64, r/m64
    EXPECT_EQ(encoded[2], 0x53);           // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegImm, Arith64Reg_rbxImm8) {
    auto [opcode, slash] = GetParam();
    BUILD(opcode, 64).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48);                 // REX.W
    EXPECT_EQ(encoded[1], 0x83);                 // opcode r/m64, imm8
    EXPECT_EQ(encoded[2], 0xc3 | (slash << 3u)); // modrm(0b11, slash, rbx=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegImm, Arith64Reg_r11Imm8) {
    auto [opcode, slash] = GetParam();
    BUILD(opcode, 64).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49);                 // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x83);                 // opcode r/m64, imm8
    EXPECT_EQ(encoded[2], 0xc3 | (slash << 3u)); // modrm(0b11, slash, r11=3)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST_P(ArithRegReg, Arith64Reg_raxReg_rbx) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 64).reg(Register::rax).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x48);           // REX.W
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r/m64, r64
    EXPECT_EQ(encoded[2], 0xd8);           // modrm(0b11, rbx=3, rax=0)
}

TEST_P(ArithRegReg, Arith64Reg_r11Reg_r12) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 64).reg(Register::r11).reg(Register::r12);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x4d);           // REX.W + REX.R(r12) + REX.B(r11)
    EXPECT_EQ(encoded[1], encoded_opcode); // opcode r/m64, r64
    EXPECT_EQ(encoded[2], 0xe3);           // modrm(0b11, r12=4, r11=3)
}

INSTANTIATE_TEST_SUITE_P(x86EncoderTest, ArithRegImm,
                         testing::Values(std::pair<Opcode, std::uint8_t>(Opcode::Add, 0),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Sub, 5),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Cmp, 7)));
INSTANTIATE_TEST_SUITE_P(x86EncoderTest, ArithRegReg,
                         testing::Values(std::pair<Opcode, std::uint8_t>(Opcode::Add, 0x01),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Sub, 0x29),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Cmp, 0x39)));
INSTANTIATE_TEST_SUITE_P(x86EncoderTest, ArithRegRm,
                         testing::Values(std::pair<Opcode, std::uint8_t>(Opcode::Add, 0x03),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Sub, 0x2b),
                                         std::pair<Opcode, std::uint8_t>(Opcode::Cmp, 0x3b)));

TEST(x86EncoderTest, CallOff32) {
    BUILD(Opcode::Call, 0).off(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0xe8); // call off32
    EXPECT_EQ(encoded[1], 0xfb);
    EXPECT_EQ(encoded[2], 0xff);
    EXPECT_EQ(encoded[3], 0xff);
    EXPECT_EQ(encoded[4], 0xff);
}

TEST(x86EncoderTest, Leave64) {
    BUILD_NO_OPERANDS(Opcode::Leave);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 1);
    EXPECT_EQ(encoded[0], 0xc9); // leave
}

TEST(x86EncoderTest, Mov16Reg_bxImm16) {
    BUILD(Opcode::Mov, 16).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x66); // operand size override
    EXPECT_EQ(encoded[1], 0xbb); // mov r16, imm16 = b8 + bx=3
    EXPECT_EQ(encoded[2], 0x00);
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov16Reg_r11wImm16) {
    BUILD(Opcode::Mov, 16).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0x66); // operand size override
    EXPECT_EQ(encoded[1], 0x41); // REX.B(r11w)
    EXPECT_EQ(encoded[2], 0xbb); // mov r16, imm16 = b8 + r11w=3
    EXPECT_EQ(encoded[3], 0x00);
    EXPECT_EQ(encoded[4], 0x00);
}

TEST(x86EncoderTest, Mov32Reg_ebxImm32) {
    BUILD(Opcode::Mov, 32).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 5);
    EXPECT_EQ(encoded[0], 0xbb); // mov r32, imm32 = b8 + ebx=3
    EXPECT_EQ(encoded[1], 0x00);
    EXPECT_EQ(encoded[2], 0x00);
    EXPECT_EQ(encoded[3], 0x00);
    EXPECT_EQ(encoded[4], 0x00);
}

TEST(x86EncoderTest, Mov32Reg_r11dImm32) {
    BUILD(Opcode::Mov, 32).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 6);
    EXPECT_EQ(encoded[0], 0x41); // REX.B(r11d)
    EXPECT_EQ(encoded[1], 0xbb); // mov r32, imm32 = b8 + r11d=3
    EXPECT_EQ(encoded[2], 0x00);
    EXPECT_EQ(encoded[3], 0x00);
    EXPECT_EQ(encoded[4], 0x00);
    EXPECT_EQ(encoded[5], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_rbxImm64) {
    BUILD(Opcode::Mov, 64).reg(Register::rbx).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 10);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0xbb); // mov r64, imm64 = b8 + rbx=3
    EXPECT_EQ(encoded[2], 0x00);
    EXPECT_EQ(encoded[3], 0x00);
    EXPECT_EQ(encoded[4], 0x00);
    EXPECT_EQ(encoded[5], 0x00);
    EXPECT_EQ(encoded[6], 0x00);
    EXPECT_EQ(encoded[7], 0x00);
    EXPECT_EQ(encoded[8], 0x00);
    EXPECT_EQ(encoded[9], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_r11Imm64) {
    BUILD(Opcode::Mov, 64).reg(Register::r11).imm(0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 10);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0xbb); // mov r64, imm64 = b8 + r11=3
    EXPECT_EQ(encoded[2], 0x00);
    EXPECT_EQ(encoded[3], 0x00);
    EXPECT_EQ(encoded[4], 0x00);
    EXPECT_EQ(encoded[5], 0x00);
    EXPECT_EQ(encoded[6], 0x00);
    EXPECT_EQ(encoded[7], 0x00);
    EXPECT_EQ(encoded[8], 0x00);
    EXPECT_EQ(encoded[9], 0x00);
}

// TODO: Tests for Mov16 and Mov32.

TEST(x86EncoderTest, Mov64Base_rbpDisp8Reg_rax) {
    BUILD(Opcode::Mov, 64).base_disp(Register::rbp, 0).reg(Register::rax);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x45); // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Base_rbpDisp8Reg_r10) {
    BUILD(Opcode::Mov, 64).base_disp(Register::rbp, 0).reg(Register::r10);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c); // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x55); // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Base_r11Disp8Reg_rax) {
    BUILD(Opcode::Mov, 64).base_disp(Register::r11, 0).reg(Register::rax);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x43); // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Base_r11Disp8Reg_r10) {
    BUILD(Opcode::Mov, 64).base_disp(Register::r11, 0).reg(Register::r10);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0x53); // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_raxBase_rbpDisp8) {
    BUILD(Opcode::Mov, 64).reg(Register::rax).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x45); // modrm(0b01, rax=0, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_raxBase_r11Disp8) {
    BUILD(Opcode::Mov, 64).reg(Register::rax).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x49); // REX.W + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x43); // modrm(0b01, rax=0, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_r10Base_rbpDisp8) {
    BUILD(Opcode::Mov, 64).reg(Register::r10).base_disp(Register::rbp, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4c); // REX.W + REX.R(r10)
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x55); // modrm(0b01, r10=2, [rbp]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_r10Base_r11Disp8) {
    BUILD(Opcode::Mov, 64).reg(Register::r10).base_disp(Register::r11, 0);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r10) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x8b); // mov r64, r/m64
    EXPECT_EQ(encoded[2], 0x53); // modrm(0b01, r10=2, [r11-8=rbx]+disp8)
    EXPECT_EQ(encoded[3], 0x00);
}

TEST(x86EncoderTest, Mov64Reg_raxReg_rbx) {
    BUILD(Opcode::Mov, 64).reg(Register::rax).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x48); // REX.W
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0xd8); // modrm(0b11, rbx=3, rax=0)
}

TEST(x86EncoderTest, Mov64Reg_r11Reg_r12) {
    BUILD(Opcode::Mov, 64).reg(Register::r11).reg(Register::r12);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x4d); // REX.W + REX.R(r12) + REX.B(r11)
    EXPECT_EQ(encoded[1], 0x89); // mov r/m64, r64
    EXPECT_EQ(encoded[2], 0xe3); // modrm(0b11, r12=4, r11=3)
}

TEST(x86EncoderTest, PopReg_rbx) {
    BUILD(Opcode::Pop, 64).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 1);
    EXPECT_EQ(encoded[0], 0x5b); // pop r64 = 58 + rbx=3
}

TEST(x86EncoderTest, PopReg_r11) {
    BUILD(Opcode::Pop, 64).reg(Register::r11);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 2);
    EXPECT_EQ(encoded[0], 0x41); // REX.B
    EXPECT_EQ(encoded[1], 0x5b); // pop r64 = 58 + r11=3
}

TEST(x86EncoderTest, PushReg_rbx) {
    BUILD(Opcode::Push, 64).reg(Register::rbx);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 1);
    EXPECT_EQ(encoded[0], 0x53); // push r64 = 50 + rbx=3
}

TEST(x86EncoderTest, PushReg_r11) {
    BUILD(Opcode::Push, 64).reg(Register::r11);
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
    BUILD(opcode, 8).reg(Register::rax);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 3);
    EXPECT_EQ(encoded[0], 0x0f);           // secondary opcode table
    EXPECT_EQ(encoded[1], encoded_opcode); // setcc r/m
    EXPECT_EQ(encoded[2], 0xc0);           // modrm(0b11, 0, al=0)
}

TEST_P(Setcc, Setcc_spl) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 8).reg(Register::rsp);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x40);           // REX
    EXPECT_EQ(encoded[1], 0x0f);           // secondary opcode table
    EXPECT_EQ(encoded[2], encoded_opcode); // setcc r/m
    EXPECT_EQ(encoded[3], 0xc4);           // modrm(0b11, 0, spl=4)
}

TEST_P(Setcc, Setcc_sil) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 8).reg(Register::rsi);
    auto [encoded, length] = encode(inst);
    EXPECT_EQ(length, 4);
    EXPECT_EQ(encoded[0], 0x40);           // REX
    EXPECT_EQ(encoded[1], 0x0f);           // secondary opcode table
    EXPECT_EQ(encoded[2], encoded_opcode); // setcc r/m
    EXPECT_EQ(encoded[3], 0xc6);           // modrm(0b11, 0, sil=6)
}

TEST_P(Setcc, Setcc_r11b) {
    auto [opcode, encoded_opcode] = GetParam();
    BUILD(opcode, 8).reg(Register::r11);
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

} // namespace
} // namespace coel::x86
