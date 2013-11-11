#include "IdlefunctionElimination.hxx"
#include "IFEVisitor.hxx"

void IdlefunctionElimination::runPass(Design* design){

  //get the function-level DFG
  DfeGraph* dfe = design->getDfeGraph();

  IFEVisitor ifeVisitor(dfe);
  //each function node goes through HLA to extract function properties
  ifeVisitor.ExtractProperties();

  //assign dependency level based on data dependency and interconnections
  ifeVisitor.ATAPLevel();
  

  //combine tasks into segments 


  //combine segments into configurations


  //optimise configuration 
  

  //link configurations
}

string IdlefunctionElimination::logPass() {
    return "Analysing kernel properties";
}


