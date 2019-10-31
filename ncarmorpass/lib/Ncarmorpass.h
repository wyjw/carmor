//==============================================================================
// FILE:
//    MBAAdd.h
//
// DESCRIPTION:
//    Declares the MBAAdd pass for the new and the legacy pass managers.
//
// License: MIT
//==============================================================================

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

void visitor(llvm::Module &M) {
    llvm::errs() << "Visiting: ";
    llvm::errs() << M.getName() << " (takes ";
}

/*
//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
struct MBAAdd : public llvm::PassInfoMixin<MBAAdd> {
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &){
      visitor(M);
      return llvm::PreservedAnalyses::all();
  }
  bool runOnModule(llvm::Module &M);
};
*/

//------------------------------------------------------------------------------
// Legacy PM interface
//------------------------------------------------------------------------------
struct LegacyMBAAdd : public llvm::ModulePass {
  static char ID;
  LegacyMBAAdd() : ModulePass(ID) {}
  bool runOnModule(llvm::Module &M) override {
    llvm::errs() << "RUNNING MODULE\n";
    return false;
  };
};
