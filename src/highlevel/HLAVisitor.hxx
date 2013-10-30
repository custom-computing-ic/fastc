#include "../precompiled.hxx"
#include "../Kernel.hxx"
#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DataFlowGraph/Node.hxx"
#include "../DataFlowGraph/DFSVisitor.hxx"

#include <string>

class HLAVisitor {

  Kernel *kernel;
  DataFlowGraph *dfg;
  
  public:
  HLAVisitor(Kernel *k);
  void MemoryAnalysis();

};
