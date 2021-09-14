#pragma once

namespace ir {

class Function;

} // namespace ir

namespace codegen {

void register_allocate(ir::Function &function);

} // namespace codegen
