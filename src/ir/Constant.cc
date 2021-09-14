#include <ir/Constant.hh>

#include <memory>
#include <vector>

namespace ir {
namespace {

std::vector<std::unique_ptr<ir::Constant>> s_constants;

} // namespace

Constant *Constant::get(std::size_t value) {
    return s_constants.emplace_back(new Constant(value)).get();
}

} // namespace ir
