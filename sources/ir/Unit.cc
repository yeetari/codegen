#include <coel/ir/Unit.hh>

namespace coel::ir {

Function *Unit::append_function(std::string name, const Type *return_type, std::span<const Type *> parameters) {
    return m_functions.emplace<Function>(m_functions.end(), std::move(name), return_type, parameters);
}

Function *Unit::find_function(std::string_view name) {
    for (auto *function : *this) {
        if (function->name() == name) {
            return function;
        }
    }
    return nullptr;
}

} // namespace coel::ir
