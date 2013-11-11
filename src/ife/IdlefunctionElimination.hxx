#ifndef IDLEFUNCTIONELIMINATION_HXX_
#define IDLEFUNCTIONELIMINATION_HXX_

#include "../precompiled.hxx"
#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DataFlowGraph/Node.hxx"
#include "../DataFlowGraph/DFSVisitor.hxx"
#include "../passes/Pass.hxx"
#include "../Design.hxx"

#include <string>


class IdlefunctionElimination : public Pass {

  public:
    IdlefunctionElimination(){}
    void runPass(Design* design);
    string logPass();
};

#endif /* IDLEFUNCTIONELIMINATION_HXX_ */
