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

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(Rewriter &R1, Rewriter &R2) : TheRewriter1(R1), TheRewriter2(R2) {}

  bool VisitDecl(Decl* d)
  {
      ASTContext& ctx = d->getASTContext();
      SourceManager& sm = ctx.getSourceManager();

      const RawComment* rc = d->getASTContext().getRawCommentForDeclNoCache(d);
      if (rc)
      {
          std::cout << "found comment!" << '\n';

          //Found comment!
          SourceRange range = rc->getSourceRange();

          PresumedLoc startPos = sm.getPresumedLoc(range.getBegin());
          PresumedLoc endPos = sm.getPresumedLoc(range.getEnd());

          std::string raw = rc->getRawText(sm);
          std::string brief = rc->getBriefText(ctx);

          // Debugging printouts
          std::cout << "raw:" << raw << "\n";
          std::cout << "brief:" << brief << "\n";

          // ... Do something with positions or comments
          if(raw.find("@1") != std::string::npos) {
              std::cout << "found 1!" << '\n';
          }
          if (raw.find("@2") != std::string::npos) {
              std::cout << "found 2!" << "\n";
          }
      }

      // ...
      return true;
  }

  bool VisitFunctionDecl(FunctionDecl *f)
  {
    // Only function definitions (with bodies), not declarations.
    if (f->hasBody()) {
      Stmt *FuncBody = f->getBody();

      // Type name as string
      QualType QT = f->getReturnType();
      std::string TypeStr = QT.getAsString();

      // Function name
      DeclarationName DeclName = f->getNameInfo().getName();
      std::string FuncName = DeclName.getAsString();

      // Add comment before
      std::stringstream SSBefore;
      SSBefore << "// Begin function " << FuncName << " returning " << TypeStr
               << "\n";
      SourceLocation ST = f->getSourceRange().getBegin();
      TheRewriter1.InsertText(ST, SSBefore.str(), true, true);

      // And after
      std::stringstream SSAfter;
      SSAfter << "\n// End function " << FuncName;
      ST = FuncBody->getEndLoc().getLocWithOffset(1);
      TheRewriter2.InsertText(ST, SSAfter.str(), true, true);
    }
    return true;
  }

private:
  Rewriter &TheRewriter1;
  Rewriter &TheRewriter2;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R1, Rewriter &R2) : Visitor (R1, R2) {}

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
    SourceManager &SM1 = TheRewriter1.getSourceMgr();
    SourceManager &SM2 = TheRewriter2.getSourceMgr();
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM1.getFileEntryForID(SM1.getMainFileID())->getName() << "\n";
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM2.getFileEntryForID(SM2.getMainFileID())->getName() << "\n";

    // Now emit the rewritten buffer.
    TheRewriter1.getEditBuffer(SM1.getMainFileID()).write(llvm::outs());
    TheRewriter2.getEditBuffer(SM2.getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    TheRewriter1.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    TheRewriter2.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<MyASTConsumer>(TheRewriter1, TheRewriter2);
  }

private:
  Rewriter TheRewriter1;
  Rewriter TheRewriter2;
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
