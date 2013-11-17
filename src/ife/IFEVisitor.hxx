#ifndef IFEVISITOR_HXX_
#define IFEVISITOR_HXX_

#include "Segment.h"
#include "Configuration.h"
#include "Partition.h"

#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DfeTask.hxx"

#include <string>
#include <vector>
#include <time.h>


class IFEVisitor {

  DataFlowGraph *dfg;
  std::vector<DfeTask*> atapTasks;//tasks assigned ATAP levels
  std::vector<Segment*> levels;
  std::vector<Configuration*> configurations;
  std::vector<Partition*> partitions;

  std::vector<int> levelNums;
  std::vector<Partition*> configurationGraph;

  //to guide the search
  std::vector<Configuration*> seenConfigurations;

  //to guide the resource accumulation
  std::vector<DfeTask*> seenTasks;

  int parnum;

  public:
  IFEVisitor(DataFlowGraph *dfg);
  void ExtractProperties();
  void ATAPLevel();
  void CombineTasks();
  void CombineSegments();
  void OptimiseConfigurations();
  void GenerateSolutions();
  void EvaluateSolutions();
  void AssignLevel(DfeTask* task);
  Offset* FindSink(DfeTask* task, string name);
  void FindSource(DfeTask* task);
  string MatchName(DfeTask* root, DfeTask* branch);
  int FindOutput(DfeTask* task, Configuration* con, string name);
  void FindPartition(int start, Partition* par);
  bool seenConfiguration(Configuration* con);
  bool FindLevel(Segment* level, Configuration* con);
  bool FindOverlappedKernel(DfeTask* task);
  std::vector<Configuration*> getConfigurations() { return configurations; }
  std::vector<Partition*> getPartitions() { return partitions; }
};



#endif /* IFEVISITOR_HXX_ */
