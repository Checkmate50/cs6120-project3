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
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      for (auto& B : F) {
        for (auto& I : B) {
          if (auto* op = dyn_cast<BinaryOperator>(&I)) {
            // Insert at the point where the instruction `op` appears.
            IRBuilder<> builder(op);

            // Make a multiply with the same operands as `op`.
            Value* lhs = op->getOperand(0);
            Value* rhs = op->getOperand(1);
            Value* newOp = NULL;
            std::string opCode = op->getOpcodeName();
            if (opCode == "add") {
              newOp = builder.CreateFMul(lhs, rhs);
            }

            if (!newOp) {
              return false;
            }

            // Everywhere the old instruction was used as an operand, use our
            // new multiply instruction instead.
            for (auto& U : op->uses()) {
              User* user = U.getUser();  // A User is anything with operands.
              user->setOperand(U.getOperandNo(), newOp);
            }

            // We modified the code.
            return true;
      }}}
      return false;
    }
  };
}

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
static RegisterPass<SkeletonPass> X("skeleton", "a useless pass");