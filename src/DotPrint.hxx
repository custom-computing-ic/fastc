#ifndef DOTPRINT_HXX_
#define DOTPRINT_HXX_

#include "DataFlowGraph/DataFlowGraph.hxx"
#include "DotDFSVisitor.hxx"

namespace DotPrint {
  
  void writeDotForDfg(string filename, DataFlowGraph *dfg); 
  
}

#endif
