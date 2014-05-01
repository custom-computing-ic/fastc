#include "DotPrint.hxx"

void DotPrint::writeDotForDfg(ofstream &f, DataFlowGraph *dfg) {
  DotDFSVisitor dfsVisitor(dfg);
  dfsVisitor.traverse();
  dfsVisitor.writeDotToFile(f);
}

void DotPrint::writeDotEdges(std::ofstream &f, DataFlowGraph *dfg) {
  DotDFSVisitor dfsVisitor(dfg);
  dfsVisitor.traverse();
  dfsVisitor.writeDotEdges(f);
}
