#ifndef DOTDFSVISITOR_CXX
#define DOTDFSVISITOR_CXX

#include "DataFlowGraph/DFSVisitor.cxx"

class DotDFSVisitor : public DFSVisitor {

  string dot;

public:
  DotDFSVisitor(DataFlowGraph *dfg) : DFSVisitor(dfg) {}

  void visit(Node *n) {
    list<Node *>::iterator it;
    list<Node *> neighbours = n->getNeighbours();
    for (it = neighbours.begin(); it != neighbours.end(); it++)
      dot += n->toDot() + " -> " + (*it)->toDot() + ";\n";
  }

  void beforeVisit() {
    dot += "digraph {\n";
  }

  void afterVisit() {
    dot += "}";
  }

  string getDot() {
    return dot;
  }
};

#endif 
