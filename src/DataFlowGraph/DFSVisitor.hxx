#ifndef DFSVISITOR_HXX
#define DFSVISITOR_HXX

#include "DataFlowGraph.hxx"
#include "../precompiled.hxx"
#include <list>

class DFSVisitor {
  DataFlowGraph *dfg;
  std::list<Node*> queue, seenNodes;
  std::list<Node*> nodes;
  int seen;

public:
  bool seenNode(Node* n);
  DFSVisitor(DataFlowGraph *dfg);
  void traverse();
  virtual void visit(Node *n)=0;
  virtual void beforeVisit() {}
  virtual void afterVisit()  {}

};

#endif
