#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
using namespace llvm;

struct p32 {
  int val1;
  int val2;
};

namespace {
  struct FloatPass : public FunctionPass {
    static char ID;
    FloatPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LLVMContext &Ctx = F.getContext();

      bool modified = false;
      for (auto& B : F) {
        for (auto& I : B) {
          auto* op = &I;
          // Insert at the point where the instruction `op` appears.
          IRBuilder<> builder(op);
          builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
          Value* newOp = NULL;
          std::string opCode = op->getOpcodeName();

          // Make a multiply with the same operands as `op`.
          if (op->getNumOperands() == 2) {
            Value* lhs = op->getOperand(0);
            Value* rhs = op->getOperand(1);
            // If we're doing a binary double operation, lower it and raise it again
            if (lhs->getType()->isDoubleTy() && rhs->getType()->isDoubleTy()) {
              // Again, lots of refactoring that could be done, but...
              Value* args[] = {lhs, rhs};
              if (opCode == "fadd") {
                Value* leftOp = builder.CreateFPTrunc(lhs, Type::getFloatTy(Ctx));
                Value* rightOp = builder.CreateFPTrunc(rhs, Type::getFloatTy(Ctx));
                Value* addOp = builder.CreateFAdd(leftOp, rightOp);
                newOp = builder.CreateFPExt(addOp, Type::getDoubleTy(Ctx));
              }
              if (opCode == "fsub") {
                Value* leftOp = builder.CreateFPTrunc(lhs, Type::getFloatTy(Ctx));
                Value* rightOp = builder.CreateFPTrunc(rhs, Type::getFloatTy(Ctx));
                Value* addOp = builder.CreateFSub(leftOp, rightOp);
                newOp = builder.CreateFPExt(addOp, Type::getDoubleTy(Ctx));
              }
              if (opCode == "fmul") {
                Value* leftOp = builder.CreateFPTrunc(lhs, Type::getFloatTy(Ctx));
                Value* rightOp = builder.CreateFPTrunc(rhs, Type::getFloatTy(Ctx));
                Value* addOp = builder.CreateFMul(leftOp, rightOp);
                newOp = builder.CreateFPExt(addOp, Type::getDoubleTy(Ctx));
              }
              if (opCode == "fdiv") {
                Value* leftOp = builder.CreateFPTrunc(lhs, Type::getFloatTy(Ctx));
                Value* rightOp = builder.CreateFPTrunc(rhs, Type::getFloatTy(Ctx));
                Value* addOp = builder.CreateFDiv(leftOp, rightOp);
                newOp = builder.CreateFPExt(addOp, Type::getDoubleTy(Ctx));
              }
              if (opCode == "feq") {
                Value* leftOp = builder.CreateFPTrunc(lhs, Type::getFloatTy(Ctx));
                Value* rightOp = builder.CreateFPTrunc(rhs, Type::getFloatTy(Ctx));
                Value* addOp = builder.CreateFCmpOEQ(leftOp, rightOp);
                newOp = builder.CreateFPExt(addOp, Type::getDoubleTy(Ctx));
              }
              if (opCode == "flt") {
                Value* leftOp = builder.CreateFPTrunc(lhs, Type::getFloatTy(Ctx));
                Value* rightOp = builder.CreateFPTrunc(rhs, Type::getFloatTy(Ctx));
                Value* addOp = builder.CreateFCmpOLT(leftOp, rightOp);
                newOp = builder.CreateFPExt(addOp, Type::getDoubleTy(Ctx));
              }
              if (opCode == "fle") {
                Value* leftOp = builder.CreateFPTrunc(lhs, Type::getFloatTy(Ctx));
                Value* rightOp = builder.CreateFPTrunc(rhs, Type::getFloatTy(Ctx));
                Value* addOp = builder.CreateFCmpOLE(leftOp, rightOp);
                newOp = builder.CreateFPExt(addOp, Type::getDoubleTy(Ctx));
              }
            }
          }

          if (!newOp) {
            continue;
          }

          // Everywhere the old instruction was used as an operand, use our
          // new multiply instruction instead.
          for (auto& U : op->uses()) {
            User* user = U.getUser();  // A User is anything with operands.
            user->setOperand(U.getOperandNo(), newOp);
          }
          modified = true;
      }}
      return modified;
    }
  };
}

char FloatPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerFloatPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new FloatPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerFloatPass);
static RegisterPass<FloatPass> X("float", "converts double point operations to float operations");
