#ifndef DOTDFSVISITOR_HXX
#define DOTDFSVISITOR_HXX

#include "precompiled.hxx"
#include "DataFlowGraph/DFSVisitor.hxx"

#include <string>
#include <fstream>

class DataFlowGraph;

class DotDFSVisitor : public DFSVisitor {

  string dot;

public:
  DotDFSVisitor(DataFlowGraph *dfg) : DFSVisitor(dfg) {}

  void visit(Node *n);
  void beforeVisit();
  void afterVisit();
  string getDot();
  void writeDotToFile(std::ofstream &f);
  void writeDotEdges(ofstream &f);
};

#endif
