#include "BuildDFG.hxx"
#include "../AstToDfgVisitor.hxx"
#include "../StencilAstToDfgVisitor.h"

void BuildDFG::runPass(Design *design) {
  foreach_(Kernel *k, design->getKernels()) {
    if (k->isStencilKernel()) {
      StencilAstToDfgVisitor visitor(k);
      visitor.traverse(k->getDeclaration(), preorder);
    } else {
      ASTtoDFGVisitor visitor(k);
      visitor.traverse(k->getDeclaration(), preorder);
    }
  }
}

string BuildDFG::logPass() {
  return "Building DFG";
}
