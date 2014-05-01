#include "IdlefunctionElimination.hxx"
#include "IFEVisitor.hxx"

void IdlefunctionElimination::runPass(Design* design){

  //get the function-level DFG
  DataFlowGraph* dfg = design->getDataFlowGraph();

  IFEVisitor ifeVisitor(dfg);
  //each function node goes through HLA to extract function properties
  cout<<" (1) Extracting kernel properties"<<endl;
  ifeVisitor.ExtractProperties();

  //assign dependency level based on data dependency and interconnections
  cout<<" (2) Grouping kernels based on analysed data dependency"<<endl;
  ifeVisitor.ATAPLevel();

  //combine tasks into segments
  cout<<" (3) Combining kernels in the same ATAP level into a segment"<<endl;
  ifeVisitor.CombineTasks();

  //combine segments into configurations
  cout<<" (4) Combining segments to contruct various configurations"<<endl;
  ifeVisitor.CombineSegments();

  //optimise configuration
  cout<<" (5) Optimise constructed configurations"<<endl;
  ifeVisitor.OptimiseConfigurations();

  //link configurations
  cout<<" (6) Link optimised configurations as run-time solutions"<<endl;
  ifeVisitor.GenerateSolutions();

  //evaluate generated partitions
  cout<<" (7) Evaluate each solution"<<endl;
  ifeVisitor.EvaluateSolutions();

  design->setPartitions(ifeVisitor.getPartitions());
}

string IdlefunctionElimination::logPass() {
    return "Back-end analyses and optimisation";
}
