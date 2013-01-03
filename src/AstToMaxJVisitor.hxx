#ifndef ASTTOMAXJVISITOR_HXX
#define ASTTOMAXJVISITOR_HXX
#include "precompiled.hxx"

#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>

#include "DataFlowGraph/Node.hxx"
#include "DataFlowGraph/InputNode.hxx"
#include "DataFlowGraph/OutputNode.hxx"
#include "DataFlowGraph/DataFlowGraph.hxx"

#include "Kernel.hxx"

using namespace std;
using namespace boost;

class ASTtoMaxJVisitor : public ROSE_VisitorPattern {
  list<InputNode> inputs;
  list<OutputNode> outputs;
  regex *PRAGMA_IN, *KERNEL_FUNC, *PRAGMA_OUT, *PRAGMA_SCALAR_IN;

  DataFlowGraph dfg;

  string source;

  map<string, string> counterMap;

  string declarations;

  list<Kernel *> kernels;
  Kernel* currentKernel;

  void function_call_initializer(string&, SgFunctionCallExp*);
  string* toExprRec(SgExpression*);

public:
  ASTtoMaxJVisitor();
  //virtual void visit (SgNode*);
  virtual void atTraversalEnd();
  virtual void atTraversalStart();

  //void visit(SgFunctionDeclaration* functionDeclaration);
  void visit(SgVariableDeclaration*);
  void visit(SgPragma*);
  void visit(SgFunctionCallExp*);

  void writeKernels(ostream&);

  string* toExpr(SgExpression *);
  string imports();
  string kernelName();
  string declaration();
  string import(list<string>);
  string getSource() {
	  return declarations + "\n\n" + source + "\n}\n}";
  }
};
#endif
