#ifndef DOTPRINT_HXX_
#define DOTPRINT_HXX_

#include "DataFlowGraph/DataFlowGraph.hxx"
#include "DotDFSVisitor.hxx"

#include <fstream>

namespace DotPrint {

  void writeDotForDfg(std::ofstream &f, DataFlowGraph *dfg);
  void writeDotEdges(std::ofstream &f, DataFlowGraph *dfg);

}

#endif
