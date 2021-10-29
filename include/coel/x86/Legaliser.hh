#pragma once

namespace coel::codegen {

class Context;

} // namespace coel::codegen

namespace coel::x86 {

void legalise(codegen::Context &context);

} // namespace coel::x86
