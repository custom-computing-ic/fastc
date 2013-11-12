#ifndef TopSortVISITOR_HXX
#define TopSortVISITOR_HXX

#include "DataFlowGraph.hxx"
#include "../precompiled.hxx"
#include <list>
#include <vector>

class TopSortVisitor {
public:
  DataFlowGraph *dfg;
  std::vector<Node*> queue, seenNodes;
  std::vector<Node*> nodes;
  int seen;

  bool seenNode(Node* n);
  TopSortVisitor(DataFlowGraph *dfg);
  void traverse();
  void sort(Node* n);
  virtual void visit(Node *n)=0;
  virtual void beforeVisit() {}
  virtual void afterVisit()  {}

};

#endif
