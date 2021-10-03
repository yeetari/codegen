#include <coel/ir/BasicBlock.hh>

#include <coel/ir/Instruction.hh>

namespace coel::ir {

BasicBlock::~BasicBlock() = default;

bool BasicBlock::empty() const {
    return m_instructions.empty();
}

bool BasicBlock::has_terminator() const {
    if (empty()) {
        return false;
    }
    return (--end())->is_terminator();
}

} // namespace coel::ir
