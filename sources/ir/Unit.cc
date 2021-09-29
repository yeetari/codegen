#include <codegen/ir/Unit.hh>

namespace ir {

Function *Unit::append_function(std::string name, std::size_t argument_count) {
    return m_functions.emplace<Function>(m_functions.end(), std::move(name), argument_count);
}

} // namespace ir
