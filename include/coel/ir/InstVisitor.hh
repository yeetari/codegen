#pragma once

namespace coel::ir {

class BinaryInst;
class BranchInst;
class CallInst;
class CompareInst;
class CondBranchInst;
class CopyInst;
class LoadInst;
class RetInst;
class StoreInst;

struct InstVisitor {
    virtual void visit(BinaryInst *) = 0;
    virtual void visit(BranchInst *) = 0;
    virtual void visit(CallInst *) = 0;
    virtual void visit(CompareInst *) = 0;
    virtual void visit(CondBranchInst *) = 0;
    virtual void visit(CopyInst *) = 0;
    virtual void visit(LoadInst *) = 0;
    virtual void visit(RetInst *) = 0;
    virtual void visit(StoreInst *) = 0;
};

} // namespace coel::ir
