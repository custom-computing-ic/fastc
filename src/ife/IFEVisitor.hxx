#ifndef IFEVISITOR_HXX_
#define IFEVISITOR_HXX_

#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DfeTask.hxx"

#include <string>
#include <vector>
class Segment{
  
  std::vector<DfeTask*> segment; 

  public:
  Segment(){};
  void addTask(DfeTask* task){ segment.push_back(task);}
  std::vector<DfeTask*> getTasks(){ return segment;}

};

class Configuration{
  
  std::vector<Segment*> configuration;
  string name;

  public:
  int level;
  double BRAMs, LUTs, FFs, DSPs;
  double bandwidth;
  Configuration(string Cname){BRAMs=0; LUTs=0; FFs=0; DSPs=0; bandwidth=0; name = Cname;};
  void addSegment(Segment* segment){ configuration.push_back(segment);}
  std::vector<Segment*> getConfiguration(){ return configuration;}
  string getName(){return name;}
  void setName(string Cname){name = Cname;}
};

class Partition{
  
  std::vector<Configuration*> partition;
  public:
  Partition(){};
  void addConfiguration(Configuration* conf){ partition.push_back(conf);}
  void popConfiguration(){ partition.pop_back();}
  std::vector<Configuration*> getPartition(){ return partition;}
};

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
  bool FindOutput(DfeTask* task, Configuration* con);
  void FindPartition(int start, Partition* par);
  bool seenConfiguration(Configuration* con);
};



#endif /* IFEVISITOR_HXX_ */
