#pragma once

#include <x86/MachineInst.hh>

#include <cstdint>
#include <memory>
#include <vector>

namespace ir {

class Function;

} // namespace ir

namespace x86 {

std::vector<MachineInst> compile(const ir::Function &function);
std::vector<std::uint8_t> encode(const std::vector<MachineInst> &insts);

} // namespace x86
