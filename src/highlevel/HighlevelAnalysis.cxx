#include "HighlevelAnalysis.hxx"
#include "HLAVisitor.hxx"

void HighlevelAnalysis::runPass(Design* design){
  foreach_(Kernel* k, design->getKernels()) {
    HLAVisitor hlaVisitor(k);
    hlaVisitor.OnchipMemoryAnalysis();
    hlaVisitor.OffchipCommunicationAnalysis();
    hlaVisitor.ArithmeticAnalysis();
  }
}

string HighlevelAnalysis::logPass() {
    return "Analysing kernel properties";
}


