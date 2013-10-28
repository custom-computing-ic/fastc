#include "PrintDotDFG.hxx"
#include "../DotDFSVisitor.hxx"

#include <fstream>

void PrintDotDFG::runPass(Design *design) {
  foreach_(Kernel *k, design->getKernels()) {
    string fileName = k->getName() + "_graph.dot";
    DotDFSVisitor dotVisitor(k->getDataFlowGraph());
    dotVisitor.traverse();
    string dot = dotVisitor.getDot();
    ofstream f(fileName.c_str());
    f << dot;
    f.close();
  }
}

string PrintDotDFG::logPass() {
  return "Printing DOT DFG.";
}
