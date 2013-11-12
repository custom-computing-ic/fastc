#include "IdlefunctionElimination.hxx"
#include "IFEVisitor.hxx"

void IdlefunctionElimination::runPass(Design* design){

  //get the function-level DFG
  DataFlowGraph* dfg = design->getDataFlowGraph();

  IFEVisitor ifeVisitor(dfg);
  //each function node goes through HLA to extract function properties
  ifeVisitor.ExtractProperties();

  //assign dependency level based on data dependency and interconnections
  ifeVisitor.ATAPLevel();
  
  //combine tasks into segments
  ifeVisitor.CombineTasks(); 

  //combine segments into configurations
  ifeVisitor.CombineSegments();

  //optimise configuration 
  ifeVisitor.OptimiseConfigurations(); 

  //link configurations
  ifeVisitor.GenerateSolutions(); 
}

string IdlefunctionElimination::logPass() {
    return "Analysing kernel properties";
}


