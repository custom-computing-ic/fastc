#ifndef MAXJVSITOR_CXX
#define MAXJVISITOR_CXX

#include "DataFlowGraph/BFSVisitor.cxx"

class MaxJVisitor : public BFSVisitor {
    string code;

public:
  MaxJVisitor(DataFlowGraph *dfg) : BFSVisitor(dfg) {};
  
  void visit(Node *n) {
    code += n->toMaxJ() + ";\n";
  }

  string getMaxJCode() {
    return code;
  }

};

#endif
