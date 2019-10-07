//------------------------------------------------------------------------------
// Tooling sample. Demonstrates:
//
// * How to write a simple source tool using libTooling.
// * How to use RecursiveASTVisitor to find interesting AST nodes.
// * How to use the Rewriter API to rewrite the source code.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include <sstream>
#include <string>
#include <iostream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

bool GetMachineComment(Decl* d, std::string machineNum, ASTContext& ctx, SourceManager& sm) {

  const RawComment* rc = d->getASTContext().getRawCommentForDeclNoCache(d);
  if (rc)
  {
    SourceRange range = rc->getSourceRange();
    PresumedLoc startPos = sm.getPresumedLoc(range.getBegin());
    PresumedLoc endPos = sm.getPresumedLoc(range.getEnd());

    std::string raw = rc->getRawText(sm);
    std::string brief = rc->getBriefText(ctx);

    if (raw.find(machineNum) != std::string::npos)
    {
      return true;
    }
  }
  return false;
}

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(Rewriter &R) : TheRewriter(R) {}
  bool VisitFunctionDecl(FunctionDecl* d)
  {
    if (!d->isMain())
    {
      ASTContext& ctx = d->getASTContext();
      SourceManager& sm = ctx.getSourceManager();

      SourceRange ST_R = d->getSourceRange();
      SourceLocation ST_B = d->getSourceRange().getBegin();
      SourceLocation ST_E = d->getSourceRange().getEnd();

      if(GetMachineComment(d, "@1", ctx, sm)){
        // here
      }
      else
      {
        std::cout << ST_R.printToString(sm) << "\n";
        std::cout << TheRewriter.RemoveText(ST_R) << "\n";
        std::cout << TheRewriter.getRewrittenText(ST_R) << "\n";
      }
    }

    return true;
  }

  /*
  bool VisitStmt(Stmt *s) {
    ASTContext& ctx = s->getASTContext();
    SourceManager& sm = ctx.getSourceMgr();

    SourceRange ST_R = s->getSourceRange();
    std::string _R = s->printToString();

    raw.find(machineNum) != std::string::npo
    if(GetMachineComment(d, "@1", ctx, sm)){
      std::cout << "We have statement" << ST_R.printToString(sm) << "\n";
    }
    else
    {
      std::cout
    }

    return true;
  }
  */

  bool VisitVarDecl(VarDecl* d)
  {

    const DeclContext* _dc = d->getParentFunctionOrMethod();
    if (_dc) {
      if (_dc->isFunctionOrMethod())
      {
        FunctionDecl* f = FunctionDecl::castFromDeclContext(_dc);
        std::cout << f->getNameInfo().getAsString() << "\n";
        if (!f->isMain()) {
          std::cout << "GOT HERE! 123";
          return false;
        }
        std::cout << "DID WE GET HERE?\n";
      }
    }
    std::cout << "GOT HERE! 12";

    ASTContext& ctx = d->getASTContext();
    SourceManager& sm = ctx.getSourceManager();

    SourceRange ST_R = d->getSourceRange();
    SourceLocation ST_B = d->getSourceRange().getBegin();
    SourceLocation ST_E = d->getSourceRange().getEnd();
    ST_R.setEnd(ST_E.getLocWithOffset(4));

    if(GetMachineComment(d, "@1", ctx, sm)){
      std::cout << "GOT HERE!";
    }
    else
    {
      std::cout << ST_R.printToString(sm) << "\n";
      std::cout << TheRewriter.RemoveText(ST_R) << "\n";
      std::cout << TheRewriter.getRewrittenText(ST_R) << "\n";
    }

    return true;
  }

private:
  Rewriter &TheRewriter;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R) : Visitor(R) {}

  // Override the method that gets called for each parsed top-level
  // declaration.
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b) {
      // Traverse the declaration using our AST visitor.
      Visitor.TraverseDecl(*b);
      (*b)->dump();
    }
    return true;
  }

private:
  MyASTVisitor Visitor;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";

    // Now emit the rewritten buffer.
    TheRewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  // ClangTool::run accepts a FrontendActionFactory, which is then used to
  // create new objects implementing the FrontendAction interface. Here we use
  // the helper newFrontendActionFactory to create a default factory that will
  // return a new MyFrontendAction object every time.
  // To further customize this, we could create our own factory class.
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
