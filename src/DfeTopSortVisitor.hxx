#ifndef DFETopSortVISITOR_HXX
#define DFETopSortVISITOR_HXX

#include "precompiled.hxx"
#include "DataFlowGraph/TopSortVisitor.hxx"
#include "DfeTask.hxx"

#include <string>

class DataFlowGraph;

class DfeTopSortVisitor : public TopSortVisitor {

public:
  DfeTopSortVisitor(DataFlowGraph *dfg) : TopSortVisitor(dfg) {}

  void visit(Node *node);
  void beforeVisit();
  void afterVisit();
  string MatchName(DfeTask* root, DfeTask* branch);
  Offset* FindSink(DfeTask* task, string name);
  void FindSource(DfeTask* task);
  std::vector<Node*> getSorted(){ return seenNodes;}

};

#endif 
