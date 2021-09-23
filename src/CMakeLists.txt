target_include_directories(codegen PRIVATE .)
target_sources(codegen PRIVATE
    codegen/CopyInserter.cc
    codegen/Liveness.cc
    codegen/RegisterAllocator.cc
    ir/BasicBlock.cc
    ir/Constant.cc
    ir/Dumper.cc
    ir/Function.cc
    ir/Instructions.cc
    ir/Unit.cc
    ir/Value.cc
    support/Assert.cc
    x86/Backend.cc
    x86/Builder.cc
    x86/MachineInst.cc
    main.cc)
