#include <codegen/ir/Unit.hh>

namespace ir {

Function *Unit::append_function(std::string name, std::size_t argument_count) {
    return m_functions.emplace_back(new Function(std::move(name), argument_count)).get();
}

} // namespace ir
