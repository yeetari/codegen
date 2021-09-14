#include <ir/Function.hh>

namespace ir {

BasicBlock *Function::append_block() {
    return m_blocks.emplace_back(new BasicBlock).get();
}

} // namespace ir
