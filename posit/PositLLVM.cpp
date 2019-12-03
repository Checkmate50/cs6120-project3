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
      // Build function types

      // Lots of duplicated code I'm too lazy to factor out
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> paramTypes;
      Type* retType;
      FunctionType* funcType;
      StructType* positType = StructType::create(Ctx);
      positType->setBody({StructType::getInt64Ty(Ctx)});
      paramTypes = {Type::getDoubleTy(Ctx)};
      retType = positType;
      funcType = FunctionType::get(retType, paramTypes, false);
      Constant* positFunc = F.getParent()->getOrInsertFunction("convertDoubleToP32", funcType);
      paramTypes = {positType, positType};
      retType = positType;
      funcType = FunctionType::get(retType, paramTypes, false);
      Constant* addFunc = F.getParent()->getOrInsertFunction("p32_add", funcType);
      Constant* subFunc = F.getParent()->getOrInsertFunction("p32_sub", funcType);
      Constant* mulFunc = F.getParent()->getOrInsertFunction("p32_mul", funcType);
      Constant* divFunc = F.getParent()->getOrInsertFunction("p32_div", funcType);
      retType = Type::getInt32Ty(Ctx);
      funcType = FunctionType::get(retType, paramTypes, false);
      Constant* eqFunc = F.getParent()->getOrInsertFunction("p32_eq", funcType);
      Constant* leFunc = F.getParent()->getOrInsertFunction("p32_le", funcType);
      Constant* ltFunc = F.getParent()->getOrInsertFunction("p32_lt", funcType);
      paramTypes = {positType};
      retType = Type::getDoubleTy(Ctx);
      funcType = FunctionType::get(retType, paramTypes, false);
      Constant* lowerFunc = F.getParent()->getOrInsertFunction("convertP32ToDouble", funcType);

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
                Value* leftOp = builder.CreateCall(positFunc, {lhs});
                Value* rightOp = builder.CreateCall(positFunc, {rhs});
                Value* fnOp = builder.CreateCall(addFunc, {leftOp, rightOp});
                newOp = builder.CreateCall(lowerFunc, fnOp);
              }
              if (opCode == "fsub") {
                Value* leftOp = builder.CreateCall(positFunc, {lhs});
                Value* rightOp = builder.CreateCall(positFunc, {rhs});
                Value* fnOp = builder.CreateCall(subFunc, {leftOp, rightOp});
                newOp = builder.CreateCall(lowerFunc, fnOp);
              }
              if (opCode == "fmul") {
                Value* leftOp = builder.CreateCall(positFunc, {lhs});
                Value* rightOp = builder.CreateCall(positFunc, {rhs});
                Value* fnOp = builder.CreateCall(mulFunc, {leftOp, rightOp});
                newOp = builder.CreateCall(lowerFunc, fnOp);
              }
              if (opCode == "fdiv") {
                Value* leftOp = builder.CreateCall(positFunc, {lhs});
                Value* rightOp = builder.CreateCall(positFunc, {rhs});
                Value* fnOp = builder.CreateCall(divFunc, {leftOp, rightOp});
                newOp = builder.CreateCall(lowerFunc, fnOp);
              }
              if (opCode == "feq") {
                Value* leftOp = builder.CreateCall(positFunc, {lhs});
                Value* rightOp = builder.CreateCall(positFunc, {rhs});
                newOp = builder.CreateCall(eqFunc, {leftOp, rightOp});
              }
              if (opCode == "flt") {
                Value* leftOp = builder.CreateCall(positFunc, {lhs});
                Value* rightOp = builder.CreateCall(positFunc, {rhs});
                newOp = builder.CreateCall(ltFunc, {leftOp, rightOp});
              }
              if (opCode == "fle") {
                Value* leftOp = builder.CreateCall(positFunc, {lhs});
                Value* rightOp = builder.CreateCall(positFunc, {rhs});
                newOp = builder.CreateCall(leFunc, {leftOp, rightOp});
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
