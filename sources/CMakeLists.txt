target_sources(coel PRIVATE
    codegen/CopyInserter.cc
    codegen/Liveness.cc
    codegen/RegisterAllocator.cc
    ir/BasicBlock.cc
    ir/Constant.cc
    ir/Dumper.cc
    ir/Function.cc
    ir/Instructions.cc
    ir/StackSlot.cc
    ir/Types.cc
    ir/Unit.cc
    ir/Value.cc
    support/Assert.cc
    x86/Backend.cc
    x86/Builder.cc
    x86/MachineInst.cc)
