#ifndef ASTTODFGVISITOR_HXX
#define ASTTODFGVISITOR_HXX
#include "precompiled.hxx"

#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>

#include "DataFlowGraph/InputNode.hxx"
#include "DataFlowGraph/OutputNode.hxx"
#include "DataFlowGraph/DataFlowGraph.hxx"

using namespace std;
using namespace boost;

class ASTtoDFGVisitor : public AstSimpleProcessing {
  list<InputNode> inputs;
  list<OutputNode> outputs;
  regex *PRAGMA_IN, *KERNEL_FUNC, *PRAGMA_OUT, *PRAGMA_SCALAR_IN;

  DataFlowGraph *dfg;

  void function_call_initializer(string&, SgFunctionCallExp*);
  Node* toExprNodeRec(SgExpression*);

public:
  ASTtoDFGVisitor();
  virtual void visit (SgNode*);
  virtual void atTraversalEnd();
  Node* toNode(SgExpression*);
  Node* toExprNode(SgExpression *);
  DataFlowGraph* getDataFlowGraph();
};

#endif
