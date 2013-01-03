#ifndef DFSVISITOR_HXX
#define DFSVISITOR_HXX

using namespace std;

#include "DataFlowGraph.hxx"
#include <list>

class DFSVisitor {
  DataFlowGraph *dfg;
  list<Node*> queue, seenNodes;
  list<Node*> nodes;
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
