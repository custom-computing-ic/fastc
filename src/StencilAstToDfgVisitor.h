#ifndef STENCILASTTODFGVISITOR_HXX
#define STENCILASTTODFGVISITOR_HXX
#include "precompiled.hxx"

#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>

#include "Kernel.hxx"
#include "DataFlowGraph/InputNode.hxx"
#include "DataFlowGraph/OutputNode.hxx"
#include "DataFlowGraph/DataFlowGraph.hxx"

using namespace std;
using namespace boost;


/** Traverse a FAST kernel to produce its corresponding dataflow graph
    from the ROSE AST. */
class StencilAstToDfgVisitor : public AstSimpleProcessing {

  // the kernel this visitor is visiting
  Kernel *kernel;
  // the dataflow graph this visitor is adding information to;
  // this is taken from the kernel
  DataFlowGraph *dfg;

  void function_call_initializer(string&, SgFunctionCallExp*);
  Node* toExprNodeRec(SgExpression*);

public:
  StencilAstToDfgVisitor(Kernel *k);
  virtual void visit (SgNode*);
  virtual void atTraversalEnd();
  Node* toNode(SgExpression*);
  Node* toExprNode(SgExpression *);
};

#endif
