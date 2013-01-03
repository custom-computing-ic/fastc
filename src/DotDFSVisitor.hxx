#ifndef DOTDFSVISITOR_HXX
#define DOTDFSVISITOR_HXX

#include "DataFlowGraph/DataFlowGraph.hxx"
#include "DataFlowGraph/DFSVisitor.cxx"

#include <string>

class DotDFSVisitor : public DFSVisitor {

  string dot;

public:
  DotDFSVisitor(DataFlowGraph *dfg) : DFSVisitor(dfg) {}

  void visit(Node *n);
  void beforeVisit();
  void afterVisit();
  string getDot();
};

#endif 
