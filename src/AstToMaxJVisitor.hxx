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

class ASTtoMaxJVisitor : public AstPrePostProcessing {

    int paramCount;

    list<InputNode> inputs;

    list<OutputNode> outputs;

    DataFlowGraph dfg;
    string source;
    map<string, string> counterMap;
    string declarations;
    Kernel* currentKernel;

    string* function_call_initializer(string&, SgFunctionCallExp*);
    string* toExprRec(SgExpression*);
    string* visitFcall(SgFunctionCallExp*);
    void visitVarDecl(SgVariableDeclaration*);
    void visit(SgPragma*);
    void visitFor(SgForStatement *);
    void visitExprStmt(SgExprStatement *);

    string* visitPragma(SgPragma *n);

    string* toExpr(SgExpression *);

    string imports();
    string kernelName();
    string declaration();
    string import(list<string>);

    string constVar(string);
    string constVar(string, string);
    bool isConstant(string);
    string* get_type(SgInitializedName *);
    void ignore(SgNode *);

public:
    ASTtoMaxJVisitor();
    //    virtual void visit (SgNode*);
    void preOrderVisit(SgNode *);
    void postOrderVisit(SgNode *);

    string getSource() {
        return declarations + "\n" + source + "\n}\n}";
    }

};
#endif
