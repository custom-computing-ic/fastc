#include "../precompiled.hxx"
#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DataFlowGraph/Node.hxx"
#include "../DataFlowGraph/DFSVisitor.hxx"
#include "../passes/Pass.hxx"
#include "../Design.hxx"

#include <string>


class HighlevelAnalysis : public Pass {

public:
  HighlevelAnalysis(const Compiler& compiler) : super(compiler){}
  void runPass(Design* design);
  string logPass();
};
