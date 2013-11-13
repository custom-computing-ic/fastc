#ifndef STENCILASTTOMAXJVISITOR_HXX
#define STENCILASTTOMAXJVISITOR_HXX
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

class StencilAstToMaxJVisitor : public AstPrePostProcessing {

  int paramCount;

  list<InputNode> inputs;
  list<OutputNode> outputs;

  DataFlowGraph* dfg;
  Kernel* kernel;

  map<string, string> counterMap;
  string declarations, source;

  string* function_call_initializer(string&, SgFunctionCallExp*);
  string* toExprRec(SgExpression*);
  string* visitFcall(SgFunctionCallExp*);
  void visitVarDecl(SgVariableDeclaration*);
  void visit(SgPragma*);
  void visitFor(SgForStatement *);
  void visitExprStmt(SgExprStatement *);
  string* visitPragma(SgPragma *n);

  /* Generate code for the declarations of all variables in the
     current kernel. */
  string declaration();

  string constVar(string);
  string constVar(string, string);
  bool isConstant(string);
  string* get_type(SgInitializedName *);

  /** Mark the given Sage node to be ignored by this visitor in the
      future.*/
  void ignore(SgNode *);

  /** Recursively mark the children of the given Sage node to be
      ignored by this visitor in the future.*/
  void ignoreChildren(SgNode *n);

public:
  StencilAstToMaxJVisitor();
  StencilAstToMaxJVisitor(Kernel* kernel);

  void preOrderVisit(SgNode *);
  void postOrderVisit(SgNode *);

  string* toExpr(SgExpression *);

  /** Returns the source code generated by the last AST traversal. */
  string getSource() {
    return declarations == "" ? source : declarations + "\n" + source;
  }

};
#endif
