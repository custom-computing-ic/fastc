#ifndef MAXJVISITOR_HXX
#define MAXJVISITOR_HXX

#include "DataFlowGraph/BFSVisitor.cxx"

using namespace std;

class MaxJVisitor : public BFSVisitor {
    string code;

public:
  MaxJVisitor(DataFlowGraph *dfg) : BFSVisitor(dfg) {};
  
  void visit(Node *n);
  string getMaxJCode();

};

#endif
