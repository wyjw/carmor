//==============================================================================
// FILE:
//    MBAAdd.cpp
//
// DESCRIPTION:
//    This pass performs a substitution for 8-bit integer add
//    instruction based on this Mixed Boolean-Airthmetic expression:
//      a + b == (((a ^ b) + 2 * (a & b)) * 39 + 23) * 151 + 111
//    See formula (3) in [1].
//
// USAGE:
//    1. Legacy pass manager:
//      $ opt -load <BUILD_DIR>/lib/libMBAAdd.so --legacy-mba-add [-mba-ratio=<ratio>] <bitcode-file>
//      with the optional ratio in the range [0, 1.0].
//    2. New pass maanger:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libMBAAdd.so -passes=-"mba-add" <bitcode-file>
//      The command line option is not available for the new PM
//
// [1] "Defeating MBA-based Obfuscation" Ninon Eyrolles, Louis Goubin, Marion
//     Videau
//
// License: MIT
//==============================================================================
#include "Ncarmorpass.h"
#include "Ratio.h"
#include <stdio.h>
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "MemoryModel/PointerAnalysis.h"
#include "WPA/Andersen.h"
#include "WPA/FlowSensitive.h"
#include "Util/SVFModule.h"

#include <random>

using namespace llvm;

#define DEBUG_TYPE "mba-add"
#define CARMOR_PREFIX "__carmor_"
#define NDEBUG 0

// Pass Option declaration
static cl::opt<Ratio, false, llvm::cl::parser<Ratio>> MBARatio{
    "mba-ratio",
    cl::desc("Only apply the mba pass on <ratio> of the candidates"),
    cl::value_desc("ratio"), cl::init(1.), cl::Optional};

Module * GM = nullptr;

//-----------------------------------------------------------------------------
// Mem Intrinsic function (Replace with our own function)
//-----------------------------------------------------------------------------

Function *getMemIntrinsicFunction(MemIntrinsic *MI, StringRef name)
{
  std::string functionName = CARMOR_PREFIX + name.str();
  Function* F = GM->getFunction(functionName);

  return F;
}

void visitMemInstrinsic(MemIntrinsic *MI)
{
  IRBuilder<> IRB(MI);
  if (isa<MemTransferInst>(MI))
  {
    auto memmove_args = {IRB.CreatePointerCast(MI->getOperand(0), IRB.getInt8PtrTy()),
      IRB.CreatePointerCast(MI->getOperand(1), IRB.getInt8PtrTy()),
      IRB.CreateIntCast(MI->getOperand(2), IRB.getInt64Ty(), false) };
    IRB.CreateCall(
      isa<MemMoveInst>(MI) ?
        getMemIntrinsicFunction(MI, "memmove") :
        getMemIntrinsicFunction(MI, "memcpy"),
      memmove_args);
  }
  else if (isa<MemSetInst>(MI))
  {
    auto memset_args = {IRB.CreatePointerCast(MI->getOperand(0), IRB.getInt8PtrTy()),
      IRB.CreateIntCast(MI->getOperand(1), IRB.getInt32Ty(), false),
      IRB.CreateIntCast(MI->getOperand(1), IRB.getInt64Ty(), false)};
    IRB.CreateCall(getMemIntrinsicFunction(MI, "memset"), memset_args);
  }

  MI->eraseFromParent();
}


//-----------------------------------------------------------------------------
// Initialize Pointer Analysis
//-----------------------------------------------------------------------------
/*
void initializePointerAnalysis()
{
  SVFModule svfModule(M);
  PointerAnalysis* m_PTA = new AndersenWaveDiffWithType();
  PAG* m_PAG = m_PTA->getPAG();
  m_PAG->handleBlackHole(true);
  m_PTA->analyze(svfModule);
  m_PAG = m_PTA->getPAG();
}
*/
//-----------------------------------------------------------------------------
// Handle alloc functions
//-----------------------------------------------------------------------------

StringRef getAllocationFunctionName(StringRef name)
{
  if (name.contains("malloc"))  return "malloc";
  if (name.contains("calloc"))  return "calloc";
  if (name.contains("realloc"))  return "realloc";
  if (name.contains("memalign"))  return "memalign";
  assert(false && "[ERROR] Invalid allocation function encouterned while trying to set annotation\n");
  return "";
}

void visitAllocationCallSites(llvm::Module *M)
{
	for (auto F = M->begin(), Fend = M->end(); F != Fend; ++F)
  {
		for (auto BB = F->begin(), BBend = F->end(); BB != BBend; ++BB)
		{
			for (auto I = BB->begin(); I != BB->end();)
			{
				//debug
				auto nextIt = std::next(I);
				Instruction *ii = &*I;
				ii->print(errs());
				errs() << "\n";
        errs() << "Visiting: ";
				if (CallInst* CI = dyn_cast<CallInst>(I))
				{
					Function* F_call = dyn_cast<Function>(CI->getCalledValue()->stripPointerCasts());
					if (!F_call)
					{
						I = nextIt;
						continue;
					}
					StringRef allocationFunctionName = getAllocationFunctionName(F_call->getName());
					std::string cosmixAllocationFunctionName = CARMOR_PREFIX + allocationFunctionName.str() + "_";
					Function* cosmixAllocationFunction = M->getFunction(cosmixAllocationFunctionName);
          CI->setCalledFunction(cosmixAllocationFunction);
        }
				I = nextIt;
		  }
	 }
  }
}
//-----------------------------------------------------------------------------
// Initialize Pointer Analysis
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Dataflow Analysis
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// MBAAdd Implementation
//-----------------------------------------------------------------------------
/*
bool MBAAdd::runOnBasicBlock(BasicBlock &BB){
  bool Changed = false;
  for (auto Inst = BB.begin(), IE = BB.end(); Inst != IE; ++Inst) {
    if (AllocaInst* v = dyn_cast<AllocaInst>(Inst))
    {

    }
    else{
      continue;
    }
  }
}
*/

/*
bool LegacyMBAAdd::runOnModule(llvm::Module &M){
  errs() << "GOT TO RUN ON MODULE";
  bool Changed = false;
  for (auto Inst = M.begin(), IE = M.end(); Inst != IE; ++Inst) {
    if (AllocaInst* v = dyn_cast<AllocaInst>(Inst))
    {

    }
    else{
      continue;
    }
  }
}
*/

/*
PreservedAnalyses MBAAdd::run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &) {
  bool Changed = false;

  for (auto &BB : F) {
    errs() << "GOT HERE!";
    Changed |= runOnBasicBlock(BB);
  }
  return (Changed ? llvm::PreservedAnalyses::none()
                  : llvm::PreservedAnalyses::all());
}
*/

/*
class LegacyMBAAdd : public ModulePass
{
  public:
    static char ID;

  LegacyMBAAdd() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M)
  {
    errs() << "RUNNING PASS";
  }
}
*/

/*
bool LegacyMBAAdd::runOnModule(llvm::Module &AM) {
  bool Changed = false;
  errs() << "GOT HERE on Legacy RUn on MOdule\n";

  GM = &AM;
  for (auto &F : AM) {

    Changed |= Impl.runOnFunction(F);
  }
  visitAllocationCallSites(&AM);

  return Changed;
}
*/

/*
//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getMBAAddPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MbaAdd", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerAnalysisRegistrationCallback(
                [](StringRef Name, ModuleAnalysisManager &MAM) {
                  if (Name == "mba-add") {
                    MAM.registerPass([&] {return });
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getMBAAddPluginInfo();
}
*/

//-----------------------------------------------------------------------------
// Legacy PM Registration
//-----------------------------------------------------------------------------
char LegacyMBAAdd::ID = 0;

static RegisterPass<LegacyMBAAdd> X("legacy-mba-add",
                                    "Mixed Boolean Arithmetic Substitution",
                                    true, // doesn't modify the CFG => true
                                    false // not a pure analysis pass => false
);
