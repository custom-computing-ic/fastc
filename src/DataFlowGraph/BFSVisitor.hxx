#ifndef BFSVISITOR_HXX
#define BFSVISITOR_HXX

#include "DataFlowGraph.hxx"
#include <list>

using namespace std;

class BFSVisitor {
  DataFlowGraph *dfg;
  list<Node*> queue, seenNodes;
  list<Node*> nodes;
  int seen;

  bool seenNode(Node*);

public:
  BFSVisitor(DataFlowGraph*);
  void traverse();
  virtual void visit(Node*)=0;
  virtual void beforeVisit() {}
  virtual void afterVisit()  {}

};

#endif
