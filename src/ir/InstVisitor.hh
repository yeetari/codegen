#pragma once

namespace ir {

class AddInst;
class BranchInst;
class CondBranchInst;
class CopyInst;
class RetInst;

struct InstVisitor {
    virtual void visit(AddInst *) = 0;
    virtual void visit(BranchInst *) = 0;
    virtual void visit(CondBranchInst *) = 0;
    virtual void visit(CopyInst *) = 0;
    virtual void visit(RetInst *) = 0;
};

} // namespace ir
