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

class SgFunctionCallExp;
class SgPragma;
class SgVariableDeclaration;

class ASTtoMaxJVisitor : public AstSimpleProcessing {

    int paramCount;

    list<InputNode> inputs;
    list<OutputNode> outputs;

    DataFlowGraph dfg;
    string source;
    map<string, string> counterMap;
    string declarations;
    Kernel* currentKernel;

    void function_call_initializer(string&, SgFunctionCallExp*);
    string* toExprRec(SgExpression*);void visit(SgVariableDeclaration*);
    void visit(SgPragma*);
    void visit(SgFunctionCallExp*);

    string* toExpr(SgExpression *);

    string imports();
    string kernelName();
    string declaration();
    string import(list<string>);


public:
    ASTtoMaxJVisitor();
    virtual void visit (SgNode*);

    string getSource() {
        return declarations + "\n" + source + "\n}\n}";
    }

};
#endif
