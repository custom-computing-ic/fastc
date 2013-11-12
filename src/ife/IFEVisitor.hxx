#ifndef IFEVISITOR_HXX_
#define IFEVISITOR_HXX_

#include "../DataFlowGraph/DataFlowGraph.hxx"
#include "../DfeTask.hxx"

#include <string>
#include <vector>

class IFEVisitor {

  DataFlowGraph *dfg;
  std::vector<DfeTask*> atapTasks;//tasks assigned ATAP levels

  protected:
  int levelNum;

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
  void setLevelNum(int level) {levelNum = level;}
  int  getLevelNum(){return levelNum;}
};

class Segment{
  
  std::vector<DfeTask*> segment; 

  public:
  Segment(){};
  void addTask(DfeTask* task){ segment.push_back(task);}
  std::vector<DfeTask*> getTasks(){ return segment;}

};

class Configuration{
  
  std::vector<Segment*> configuration;
  public:
  Configuration(){};
  void addSegment(Segment* segment){ configuration.push_back(segment);}
  std::vector<Segment*> getConfiguration(){ return configuration;}
};


#endif /* IFEVISITOR_HXX_ */
