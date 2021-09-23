#pragma once

#include <x86/MachineInst.hh>

#include <cstdint>
#include <memory>
#include <vector>

namespace ir {

class Unit;

} // namespace ir

namespace x86 {

std::vector<MachineInst> compile(const ir::Unit &unit);
std::vector<std::uint8_t> encode(const std::vector<MachineInst> &insts);

} // namespace x86
