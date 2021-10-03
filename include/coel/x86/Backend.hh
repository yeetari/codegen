#pragma once

#include <coel/x86/MachineInst.hh>

#include <cstdint>
#include <memory>
#include <vector>

namespace coel::ir {

class Function;
class Unit;

} // namespace coel::ir

namespace coel::x86 {

std::vector<MachineInst> compile(const ir::Unit &unit);
std::pair<std::size_t, std::vector<std::uint8_t>> encode(const std::vector<MachineInst> &insts,
                                                         const ir::Function *entry);

} // namespace coel::x86
