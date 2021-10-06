#include <coel/codegen/Context.hh>
#include <coel/codegen/CopyInserter.hh>
#include <coel/codegen/RegisterAllocator.hh>
#include <coel/graph/DepthFirstSearch.hh>
#include <coel/graph/DotGraph.hh>
#include <coel/ir/BasicBlock.hh>
#include <coel/ir/Constant.hh>
#include <coel/ir/Dumper.hh>
#include <coel/ir/Function.hh>
#include <coel/ir/Instructions.hh>
#include <coel/ir/Types.hh>
#include <coel/ir/Unit.hh>
#include <coel/support/Assert.hh>
#include <coel/x86/Backend.hh>

#include <fmt/core.h>

#include <fstream>
#include <sstream>
#include <sys/mman.h>

using namespace coel;

int main() {
    ir::Unit unit;

    std::array<const ir::Type *, 2> callee_params{ir::IntegerType::get(32), ir::IntegerType::get(32)};
    auto *main = unit.append_function("main", ir::IntegerType::get(32), {});
    auto *callee = unit.append_function("foo", ir::IntegerType::get(32), callee_params);

    auto *main_entry = main->append_block();
    auto *var1 = main->append_stack_slot(ir::IntegerType::get(32));
    auto *var2 = main->append_stack_slot(ir::IntegerType::get(32));
    main_entry->append<ir::StoreInst>(var1, ir::Constant::get(ir::IntegerType::get(32), 10));
    main_entry->append<ir::StoreInst>(var2, main_entry->append<ir::LoadInst>(var1));
    auto *call = main_entry->append<ir::CallInst>(
        callee, std::vector<ir::Value *>{main_entry->append<ir::LoadInst>(var2),
                                         ir::Constant::get(ir::IntegerType::get(32), 20)});
    main_entry->append<ir::RetInst>(call);

    auto *callee_entry = callee->append_block();
    auto *true_dst = callee->append_block();
    auto *false_dst = callee->append_block();
    auto *add1 = callee_entry->append<ir::BinaryInst>(ir::BinaryOp::Add, ir::Constant::get(ir::IntegerType::get(32), 5),
                                                      callee->argument(0));
    callee_entry->append<ir::CondBranchInst>(ir::Constant::get(ir::BoolType::get(), 1), true_dst, false_dst);
    auto *true_add = true_dst->append<ir::BinaryInst>(ir::BinaryOp::Add, add1, callee->argument(1));
    true_dst->append<ir::RetInst>(true_add);
    auto *false_add =
        false_dst->append<ir::BinaryInst>(ir::BinaryOp::Add, add1, ir::Constant::get(ir::IntegerType::get(32), 40));
    false_dst->append<ir::RetInst>(false_add);

    fmt::print("=====\n");
    fmt::print("INPUT\n");
    fmt::print("=====\n");
    ir::dump(unit);

    codegen::Context context(unit);
    codegen::insert_copies(context);

    fmt::print("===============\n");
    fmt::print("INSERTED COPIES\n");
    fmt::print("===============\n");
    ir::dump(unit);

    codegen::register_allocate(context);

    fmt::print("===================\n");
    fmt::print("ALLOCATED REGISTERS\n");
    fmt::print("===================\n");
    ir::dump(unit);

    auto compiled = x86::compile(unit);
    auto [entry, encoded] = x86::encode(compiled, main);
    std::ofstream output_file("foo.bin", std::ios::binary | std::ios::trunc);
    output_file.write(reinterpret_cast<const char *>(encoded.data()), static_cast<std::streamsize>(encoded.size()));
    auto *code_region =
        mmap(nullptr, encoded.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(code_region, encoded.data(), encoded.size());
    return reinterpret_cast<int (*)()>(static_cast<std::uint8_t *>(code_region) + entry)();
}
