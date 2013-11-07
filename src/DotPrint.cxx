#include "DotPrint.hxx"

void DotPrint::writeDotForDfg(string filename, DataFlowGraph *dfg) {
  DotDFSVisitor dfsVisitor(dfg);
  dfsVisitor.traverse();
  dfsVisitor.writeDotToFile(filename);
}
