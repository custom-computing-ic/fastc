#include "BuildDFG.hxx"
#include "../AstToDfgVisitor.hxx"

void BuildDFG::runPass(Design *design) {
  cout << "Running build dfg " << endl;
  ASTtoDFGVisitor visitor;
  visitor.traverse(design->getProject(), preorder);
  design->setDataFlowGraph(visitor.getDataFlowGraph());
}

string BuildDFG::logPass() {
  return "Building DFG";
}
