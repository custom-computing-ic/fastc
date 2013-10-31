#include "../precompiled.hxx"
#include "../Kernel.hxx"
#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DataFlowGraph/Node.hxx"
#include "../DataFlowGraph/DFSVisitor.hxx"

#include <string>

class HLAVisitor {

  Kernel *kernel;
  DataFlowGraph *dfg;
  list<int> OnchipMemory;

  public:
  HLAVisitor(Kernel *k);
  void MemoryAnalysis();
  bool findoffset( int offset);

};
