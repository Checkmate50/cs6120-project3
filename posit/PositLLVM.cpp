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
  struct PositPass : public FunctionPass {
    static char ID;
    PositPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> paramTypes = {Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)};
      Type *retType = Type::getInt32Ty(Ctx);
      FunctionType *barFuncTyp = FunctionType::get(retType, paramTypes, false);
      Constant *barFunc = F.getParent()->getOrInsertFunction("myAdd", barFuncTyp);

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
            Value* args[] = {lhs, rhs};
            if (opCode == "add") {
              newOp = builder.CreateCall(barFunc, args);
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

char PositPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerPositPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new PositPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerPositPass);
static RegisterPass<PositPass> X("posit", "converts floating point operations to posit operations");