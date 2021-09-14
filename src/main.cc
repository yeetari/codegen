#include <fmt/core.h>

#include <codegen/Context.hh>
#include <codegen/CopyInserter.hh>
#include <codegen/RegisterAllocator.hh>
#include <graph/DepthFirstSearch.hh>
#include <graph/DotGraph.hh>
#include <ir/BasicBlock.hh>
#include <ir/Constant.hh>
#include <ir/Dumper.hh>
#include <ir/Function.hh>
#include <ir/Instructions.hh>
#include <support/Assert.hh>
#include <x86/Backend.hh>

#include <fmt/core.h>

#include <fstream>
#include <sstream>

int main() {
    //    ir::Function function;
    //    auto *block = function.append_block();
    //    auto *add1 = block->append<ir::AddInst>(ir::Constant::get(5), ir::Constant::get(10));
    //    auto *add2 = block->append<ir::AddInst>(add1, ir::Constant::get(20));
    //    auto *add3 = block->append<ir::AddInst>(add1, add2);
    //    block->append<ir::RetInst>(add3);
    //    ir::dump(function);

    ir::Function function;
    auto *entry = function.append_block();
    auto *true_dst = function.append_block();
    auto *false_dst = function.append_block();
    auto *add1 = entry->append<ir::AddInst>(ir::Constant::get(5), ir::Constant::get(10));
    entry->append<ir::CondBranchInst>(ir::Constant::get(1), true_dst, false_dst);
    auto *true_add = true_dst->append<ir::AddInst>(add1, ir::Constant::get(20));
    true_dst->append<ir::RetInst>(true_add);
    auto *false_add = false_dst->append<ir::AddInst>(add1, ir::Constant::get(40));
    false_dst->append<ir::RetInst>(false_add);

    fmt::print("=====\n");
    fmt::print("INPUT\n");
    fmt::print("=====\n");
    ir::dump(function);

    codegen::Context context(function);
    codegen::insert_copies(context);

    fmt::print("===============\n");
    fmt::print("INSERTED COPIES\n");
    fmt::print("===============\n");
    ir::dump(function);

    codegen::register_allocate(function);

    fmt::print("===================\n");
    fmt::print("ALLOCATED REGISTERS\n");
    fmt::print("===================\n");
    ir::dump(function);

    auto compiled = x86::compile(function);
    auto encoded = x86::encode(compiled);
    std::ofstream output_file("foo.bin", std::ios::binary | std::ios::trunc);
    output_file.write(reinterpret_cast<const char *>(encoded.data()), static_cast<std::streamsize>(encoded.size()));
}
