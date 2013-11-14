#include "BuildDFG.hxx"
#include "../AstToDfgVisitor.hxx"

void BuildDFG::runPass(Design *design) {
  foreach_(Kernel *k, design->getKernels()) {
    ASTtoDFGVisitor visitor(k);
    visitor.traverse(k->getDeclaration(), preorder);
  }
}

string BuildDFG::logPass() {
  return "Building DFG";
}
