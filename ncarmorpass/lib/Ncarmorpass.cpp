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

//
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
#define GENERIC_ANNOTATION_SYMBOL "generic"
#define NDEBUG 0

// Pass Option declaration
static cl::opt<Ratio, false, llvm::cl::parser<Ratio>> MBARatio{
    "mba-ratio",
    cl::desc("Only apply the mba pass on <ratio> of the candidates"),
    cl::value_desc("ratio"), cl::init(1.), cl::Optional};

// Initialize pointer Analysis
void initializePointerAnalysis(llvm::Module *M, PointerAnalysis *m_PTA, PAG *m_PAG, SVFModule *svfModule)
{
  m_PTA = new AndersenWaveDiff();
  m_PTA->disablePrintStat();
  m_PAG = m_PTA->getPAG();
  m_PAG->handleBlackHole(true);

  m_PTA->analyze(*svfModule);
  m_PAG = m_PTA->getPAG();
  llvm::errs() << "POINTER ANALYSIS\n";
}

void runDataFlowAnalysis(llvm::Module *M, DenseMap<NodeID, std::string> m_ValuesToInstrumentMap,
  DenseMap<Value*, std::string> m_AnnotatedVars, bool debug_print, PAG *m_PAG, PointerAnalysis *m_PTA)
{
  errs() << "DATAFLOW ANALYSIS\n";

  for (auto kvp : m_AnnotatedVars)
  {
    errs() << "KVP's first argument is: " << kvp.first;
    errs() << "KVP's second argument is: " << kvp.second;
    Value* annotatedVar = kvp.first;

    if (debug_print)
    {
      errs() << "Debugging [Running Pointer Analysis]";
      annotatedVar->print(errs(), true);
    }

    uint targetNode = m_PAG->getValueNode(annotatedVar);

    auto cnt = m_ValuesToInstrumentMap.count(targetNode);
    if (cnt == 0 || (cnt == 1 && m_ValuesToInstrumentMap[targetNode] == kvp.second))
    {
      m_ValuesToInstrumentMap[targetNode] = kvp.second;
    }
    else
    {
      m_ValuesToInstrumentMap[targetNode] = GENERIC_ANNOTATION_SYMBOL;
    }
  }

  for (auto nIter = m_PTA->getAllValidPtrs().begin(); nIter != m_PTA->getAllValidPtrs().end(); ++nIter)
  {
    for (auto kvp : m_AnnotatedVars)
    {
      Value * annotatedVar = kvp.first;
      uint targetNode = m_PAG->getValueNode(annotatedVar);

      if (m_PTA->alias(*nIter, targetNode) != NoAlias)
      {
        auto cnt = m_ValuesToInstrumentMap.count(targetNode);
        if (cnt == 0 || (cnt == 1 && m_ValuesToInstrumentMap[targetNode] == kvp.second))
        {
          m_ValuesToInstrumentMap[*nIter] = kvp.second;
        }
        else
        {
          m_ValuesToInstrumentMap[*nIter] = GENERIC_ANNOTATION_SYMBOL;
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------

// Mem Intrinsic function (Replace with our own function)
//-----------------------------------------------------------------------------

Function *getMemIntrinsicFunction(llvm::Module *M, MemIntrinsic *MI, StringRef name)
{
  std::string functionName = CARMOR_PREFIX + name.str();
  Function* F = M->getFunction(functionName);

  return F;
}

void visitMemInstrinsic(llvm::Module *M, MemIntrinsic *MI)
{
  llvm::IRBuilder<> IRB(MI);
  if (isa<MemTransferInst>(MI))
  {
    auto memmove_args = {IRB.CreatePointerCast(MI->getOperand(0), IRB.getInt8PtrTy()),
      IRB.CreatePointerCast(MI->getOperand(1), IRB.getInt8PtrTy()),
      IRB.CreateIntCast(MI->getOperand(2), IRB.getInt64Ty(), false) };
    IRB.CreateCall(
      isa<MemMoveInst>(MI) ?
        getMemIntrinsicFunction(M, MI, "memmove") :
        getMemIntrinsicFunction(M, MI, "memcpy"),
      memmove_args);
  }
  else if (isa<MemSetInst>(MI))
  {
    auto memset_args = {IRB.CreatePointerCast(MI->getOperand(0), IRB.getInt8PtrTy()),
      IRB.CreateIntCast(MI->getOperand(1), IRB.getInt32Ty(), false),
      IRB.CreateIntCast(MI->getOperand(1), IRB.getInt64Ty(), false)};
    IRB.CreateCall(getMemIntrinsicFunction(M, MI, "memset"), memset_args);
  }

  MI->eraseFromParent();
}

//-----------------------------------------------------------------------------
// Check if function is related to memory
//-----------------------------------------------------------------------------

bool isMemoryMappingFunction(Function * F)
{
  auto name = F->getName();
  return name.equals("mmap") || name.equals("mmap64");
}

bool isDynamicAllocationFunction(Function * F)
{
  auto name = F->getName();
  bool res = name.equals("malloc") ||
             name.equals("calloc") ||
             name.equals("realloc") ||
             name.equals("memalign") ||
             name.equals("je_malloc") ||
             name.equals("je_calloc") ||
             name.equals("tc_malloc") ||
             name.equals("tc_calloc") ||
             name.equals("tc_realloc");
  return res;
}

bool isDynamicReallocationFunction(Function * F)
{
  auto name = F->getName();
  bool res = name.equals("realloc") ||
              name.equals("je_realloc") ||
              name.equals("tc_realloc");
  return res;

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

FunctionType *getFuncTypes(llvm::Function *F)
{
	std::vector<Type*> ArgTypes;
	for (const Argument &I : F->args())
		//if (VMap.count(&I) == 0) // Maybe this works?
			ArgTypes.push_back(I.getType());

	FunctionType *FTy = FunctionType::get(F->getFunctionType()->getReturnType(), ArgTypes, F->getFunctionType()->isVarArg());
	return FTy;
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
          if (isDynamicAllocationFunction(F_call))
          {
            errs() << "Got here: \n";
  					StringRef allocationFunctionName = getAllocationFunctionName(F_call->getName());
            errs() << "here?" << allocationFunctionName << "\n";
            std::string cosmixAllocationFunctionName = CARMOR_PREFIX + allocationFunctionName.str() + "_";
  					errs() << "should be replaced with" << cosmixAllocationFunctionName << "\n";//Function* cosmixAllocationFunction = M->getFunction(cosmixAllocationFunctionName);
					  FunctionType *FT = getFuncTypes(F_call);
					  Function *NewF = Function::Create(FT, F_call->getLinkage(), F_call->getAddressSpace(), cosmixAllocationFunctionName, F->getParent());
  				  //Function *cosmixAllocationFunction = M->getOrInsertFunction(cosmixAllocationFunctionName);
            //CI->setCalledFunction(cosmixAllocationFunction);
            F_call->replaceAllUsesWith(NewF);
            //NewF->setName(cosmixAllocationFunctionName);
            F_call->eraseFromParent();
            //CI->setCalledFunction(NewF);
          }
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
// Create new memory functions
//-----------------------------------------------------------------------------

void replaceMemFunction(std::string origFuncName, std::string replaceFuncName, llvm::Module *M)
{
  Function* origFunc = M->getFunction(origFuncName);
  Function* replaceFunc = M->getFunction(replaceFuncName);
  assert(origFunc);
  assert(replaceFunc);

  origFunc->replaceAllUsesWith(replaceFunc);
}

void createMemFunction(std::string origFuncName, std::string newFuncName, llvm::Module *M)
{
  Function* F = M->getFunction(origFuncName);
  if (F)
  {
    Function::Create(cast<FunctionType>(F->getValueType()), GlobalValue::ExternalLinkage, newFuncName, M);
  }
}

void replaceMemDeclarations(std::string functionName1, std::string functionName2, llvm::Module *M)
{
  Function* F1 = M->getFunction(functionName1);
  Function* F2 = M->getFunction(functionName2);
  assert(F1);
  assert(F2);
  if (F1 && F2){
    F1->replaceAllUsesWith(F2);
  }
}

// Fill in annotations
void addAnnotations(llvm::Module *M)
{
  auto global_annos = M->getNamedGlobal("llvm.global.annotations");
  if (global_annos)
  {
    auto a = cast<ConstantArray>(global_annos->getOperand(0));
    for (int i=0; i < a->getNumOperands(); i++){
      auto e = cast<ConstantStruct>(a->getOperand(i));

    	if (auto fn = dyn_cast<Function>(e->getOperand(0)->getOperand(0))){
    	  auto anno = cast<ConstantDataArray>(cast<GlobalVariable>(e->getOperand(1)->getOperand(0))->getOperand(0))->getAsCString();
        fn->addFnAttr(anno);
    	}
    }
  }
}

void doWithAnnotations(llvm::Module *M)
{
  for (auto F = M->begin(), Fend = M->end(); F != Fend; ++F){
      for (auto attr : F->getAttributes())
      {
        auto _attr = attr.getAsString();
        std::string pattern = "carmorsplit_";
      	llvm::errs() << _attr << "\n";
        if (_attr.find(pattern) != std::string::npos)
        {
          std::string a = _attr.substr(_attr.find(pattern) + pattern.length(), 1);
          llvm::errs() << "we have value\n" << a << "\n";
          //RegisterInTable(&M, m_ValuesToInstrumentMap, m_AnnotatedVars);
        }
      	/*
      	if (F->hasFnAttribute("1"))
        {
          llvm::errs() << F->getName() << " has my attribute!\n";
        }*/
      }
  }
}

void getVarMetadata(llvm::Module *M, DenseMap<NodeID, std::string> m_ValuesToInstrumentMap,
  DenseMap<Value*, std::string> m_AnnotatedVars, bool debug_print, PAG *m_PAG, PointerAnalysis *m_PTA)
{
  for (auto F = M->begin(), Fend = M->end(); F != Fend; ++F)
  {
    for (auto BB = F->begin(), BBend = F->end(); BB != BBend; ++BB)
    {
      for (auto I = BB->begin(); I != BB->end(); I++)
      {
        if (CallInst* CI = dyn_cast<CallInst>(I))
        {
          // TODO: Not sure if this is the right value that we want
          Value *v = CI->getArgOperand(0);
          // Register in the table
          //assert(m_PAG->hasValueNode(v));
          NodeID targetNode = m_PAG->getValueNode(v);
          //m_ValuesToInstrumentMap[targetNode] = v;
          llvm::errs() << "value node is \n" << m_ValuesToInstrumentMap[targetNode] << "\n";
        }
      }
    }
  }
}

/*
void visitDeclarationAnnotations(){

}
*/
/*
void RegisterInTable(llvm::Module *M, DenseMap<uint, std::string> values,
  DenseMap<Value*, std::string> vars)
{
  values[];
}
*/

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
// This method implements what the pass does
void visitor(Function &F) {
    errs() << "Visiting: ";
    errs() << F.getName() << " (takes ";
    errs() << F.arg_size() << " args)\n";
}


bool LegacyMBAAdd::runOnModule(llvm::Module &AM) {
  bool Changed = false;
  errs() << "GOT HERE on Legacy RUn on MOdule\n";

  Module * GM = nullptr;
  bool debug = true;
  GM = &AM;

  PointerAnalysis *m_PTA = nullptr;
  PAG *m_PAG = nullptr;
  SVFModule svfModule(AM);

  initializePointerAnalysis(&AM, m_PTA, m_PAG, &svfModule);

  // Annotated parts
  addAnnotations(&AM);
  DenseMap<NodeID, std::string> m_ValuesToInstrumentMap;
  DenseMap<Value*, std::string> m_AnnotatedVars;

  for (auto &F : AM) {
    visitor(F);
    //Changed |= Impl.runOnFunction(F);
  }
  doWithAnnotations(&AM);
  // Vars for new function
  //(m_ValuesToInstrumentMap, m_AnnotatedVars, debug, m_PAG, m_PTA)
  visitAllocationCallSites(&AM);
  createMemFunction("malloc", "vvmalloc", &AM);
  runDataFlowAnalysis(&AM, m_ValuesToInstrumentMap, m_AnnotatedVars, debug, m_PAG, m_PTA);
  //replaceMemFunction("malloc", "vvmalloc", &AM);

  return Changed;
}

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
