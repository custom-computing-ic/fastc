#include "HighlevelAnalysis.hxx"
#include "HLAVisitor.hxx"

void HighlevelAnalysis::runPass(Design* design){
  foreach_(Kernel* k, design->getKernels()) {
    HLAVisitor hlaVisitor(k);
    hlaVisitor.OnchipMemoryAnalysis();
    hlaVisitor.OffchipCommunicationAnalysis();
  }
}

string HighlevelAnalysis::logPass() {
    return "Analysing kernel properties";
}


