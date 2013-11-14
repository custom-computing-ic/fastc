#ifndef IFEVISITOR_HXX_
#define IFEVISITOR_HXX_

#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DfeTask.hxx"

#include <string>
#include <vector>
class Segment{
  
  std::vector<DfeTask*> segment; 
  string name;

  public:
  Segment(string name){this->name = name;};
  void addTask(DfeTask* task){ segment.push_back(task);}
  std::vector<DfeTask*> getTasks(){ return segment;}
  string getName(){return name;}
};

class Configuration{
  
  std::vector<Segment*> configuration;
  string name;

  public:
  int level;
  double BRAMs, LUTs, FFs, DSPs;
  double bandwidth;
  double Ib, Il, If, Id;
  double Ab, Al, Af, Ad;
  double Abw;

  int P;

  Configuration(string Cname){
    BRAMs=0; LUTs=0;   FFs=0;    DSPs=0;  bandwidth=0; name = Cname;
    Ib=218;  Il=34809; If=55061; Id=0;
    Ab=1064; Al=297600;Af=595200;Ad=2016; Abw= 39400;
    P =123456; 
  };
  void addSegment(Segment* segment){ configuration.push_back(segment);}
  std::vector<Segment*> getConfiguration(){ return configuration;}
  string getName(){return name;}
  void setName(string Cname){name = Cname;}
  double getReconfigurationTime(){return 0.8;}
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
  void EvaluateSolutions();
  void AssignLevel(DfeTask* task); 
  Offset* FindSink(DfeTask* task, string name);
  void FindSource(DfeTask* task);
  string MatchName(DfeTask* root, DfeTask* branch);
  int FindOutput(DfeTask* task, Configuration* con, string name);
  void FindPartition(int start, Partition* par);
  bool seenConfiguration(Configuration* con);
  bool FindLevel(Segment* level, Configuration* con);
};



#endif /* IFEVISITOR_HXX_ */
