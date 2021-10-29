#pragma once

#include <cstdint>

namespace coel::x86 {

class Register {
    std::uint8_t m_value{0};

public:
    static constexpr std::uint8_t rax = 0;
    static constexpr std::uint8_t rcx = 1;
    static constexpr std::uint8_t rdx = 2;
    static constexpr std::uint8_t rbx = 3;
    static constexpr std::uint8_t rsp = 4;
    static constexpr std::uint8_t rbp = 5;
    static constexpr std::uint8_t rsi = 6;
    static constexpr std::uint8_t rdi = 7;
    static constexpr std::uint8_t r8 = 8;
    static constexpr std::uint8_t r9 = 9;
    static constexpr std::uint8_t r10 = 10;
    static constexpr std::uint8_t r11 = 11;
    static constexpr std::uint8_t r12 = 12;
    static constexpr std::uint8_t r13 = 13;
    static constexpr std::uint8_t r14 = 14;
    static constexpr std::uint8_t r15 = 15;

    Register() = default;
    Register(std::uint8_t value) : m_value(value) {}

    operator std::uint8_t() const { return m_value; }
};

} // namespace coel::x86
