#pragma once

#include <codegen/ir/Function.hh>
#include <codegen/support/List.hh>

#include <cstddef>
#include <memory>
#include <string>

namespace ir {

class Unit {
    List<Function> m_functions;

public:
    auto begin() const { return m_functions.begin(); }
    auto end() const { return m_functions.end(); }

    Function *append_function(std::string name, std::size_t argument_count);
};

} // namespace ir
