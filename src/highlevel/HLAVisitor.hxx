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
  int precision;
  double frequency;
  double offchipdata;
  double bandwidth;

  double Bs, Ls, Fs, Ds;
  double BRAMs, LUTs, FFs, DSPs;

  public:
  HLAVisitor(Kernel *k);
  void OnchipMemoryAnalysis();
  void OffchipCommunicationAnalysis();
  void ArithmeticAnalysis();
  void ArithmeticResource(Node *node, int* width);
  double gap(Offset* node);
  bool findoffset( int offset, Offset* node);
};
