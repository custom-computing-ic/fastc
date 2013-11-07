#include "PrintDotDFG.hxx"
#include "../DotDFSVisitor.hxx"
#include "../DotPrint.hxx"

#include <fstream>

void PrintDotDFG::runPass(Design *design) {
  foreach_(Kernel *k, design->getKernels()) {
    string fileName = k->getName() + "_graph.dot";
    DotPrint::writeDotForDfg(fileName, k->getDataFlowGraph());
  }
}

string PrintDotDFG::logPass() {
  return "Printing DOT DFG.";
}
