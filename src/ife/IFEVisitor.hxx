#ifndef IFEVISITOR_HXX_
#define IFEVISITOR_HXX_

#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DfeTask.hxx"

#include <string>
#include <vector>

class IFEVisitor {

  DataFlowGraph *dfg;

  public:
  IFEVisitor(DataFlowGraph *dfg);
  void ExtractProperties();
  void ATAPLevel();
  void CombineTasks();
  void CombineSegments();
  void OptimiseConfigurations();
  void GenerateSolutions();
  void AssignLevel(DfeTask* task); 
  Offset* FindSink(DfeTask* task, string name);
  void FindSource(DfeTask* task);
  string MatchName(DfeTask* root, DfeTask* branch);

};

#endif /* IFEVISITOR_HXX_ */
