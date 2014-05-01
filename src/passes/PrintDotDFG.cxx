#include "PrintDotDFG.hxx"
#include "../DotDFSVisitor.hxx"
#include "../DotPrint.hxx"

#include <fstream>

void PrintDotDFG::runPass(Design *design) {
  foreach_(Kernel *k, design->getKernels()) {
    string fileName = k->getName() + "_graph.dot";
    ofstream f(fileName.c_str());
    DotPrint::writeDotForDfg(f, k->getDataFlowGraph());
    f.close();
  }
}

string PrintDotDFG::logPass() {
  return "Printing DOT DFG.";
}
