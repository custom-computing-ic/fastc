#include "BuildDFG.hxx"
#include "../AstToDfgVisitor.hxx"

void BuildDFG::runPass(Design *design) {
  cout << "Running build dfg " << endl;

  foreach_(Kernel *k, design->getKernels()) {
    ASTtoDFGVisitor visitor(k);
    visitor.traverse(design->getProject(), preorder);
  }
}

string BuildDFG::logPass() {
  return "Building DFG";
}
