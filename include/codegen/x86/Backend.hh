#pragma once

#include <codegen/x86/MachineInst.hh>

#include <cstdint>
#include <memory>
#include <vector>

namespace ir {

class Function;
class Unit;

} // namespace ir

namespace x86 {

std::vector<MachineInst> compile(const ir::Unit &unit);
std::pair<std::size_t, std::vector<std::uint8_t>> encode(const std::vector<MachineInst> &insts,
                                                         const ir::Function *entry);

} // namespace x86
