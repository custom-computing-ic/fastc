#include "IFEVisitor.hxx"
#include <sstream>
#include <math.h>     
#include <vector>
#include <list>
#include <algorithm>    // std::sort
#include "../highlevel/HLAVisitor.hxx"
#include "../DfeTopSortVisitor.hxx"

IFEVisitor::IFEVisitor(DataFlowGraph *dfg){
  this->dfg = dfg;
}

void IFEVisitor::ExtractProperties(){
  
  //each function node goes through HLA to extract function properties
  foreach_(Node* node, dfg->getNodes()) {
    //DfeTask *task = dynamic_cast<DfeTask*>(node);
    DfeTask *task = (DfeTask *)node;
    if (task->getName() == "sink" || task->getName() == "source")
      continue;

    Kernel* k = task->getKernel();
    HLAVisitor hlaVisitor(k);
    hlaVisitor.OnchipMemoryAnalysis();
    hlaVisitor.OffchipCommunicationAnalysis();
    hlaVisitor.ArithmeticAnalysis();

    //property extraction
    task->BRAMs     = hlaVisitor.getBRAMs();
    task->LUTs      = hlaVisitor.getLUTs();
    task->FFs       = hlaVisitor.getFFs();
    task->DSPs      = hlaVisitor.getDSPs();
    task->bandwidth = hlaVisitor.getbandwidth();

    task->streams   = hlaVisitor.getstreams();
  }
}

Offset* IFEVisitor::FindSink(DfeTask* task, string name){
  foreach_(Offset* stream, task->streams)
  {
    if(stream->getName() == name) return stream;
  }
  return NULL;
}

void IFEVisitor::FindSource(DfeTask* task){
  //steams asides sink nodes are source nodes
  foreach_(Offset* sink, task->sinks)
  {
    string name = sink->getName();
    foreach_(Offset* stream, task->streams)
      if(stream->getName() != name ) task->sources.push_back(stream);
  }
}

string IFEVisitor::MatchName(DfeTask *root, DfeTask* branch)
{
  foreach_(string inputname, branch->getInputs())
    foreach_(string outputname, root->getOutputs())
    if(inputname == outputname) return inputname;

  return NULL;
}

void IFEVisitor::AssignLevel(DfeTask* task)
{
  Kernel* k = task->getKernel();
  list<std::string> sinks = k->getOutputNames();
  foreach_(std::string name, sinks)
    task->sinks.push_back(FindSink(task, name));

  FindSource(task);
  int sourceD=0;
  foreach_(Offset* stream, task->sources)
  {
    cout<<"source node: "<<stream->getName()<<" delay: "<<stream->delay<<endl;
    sourceD = sourceD > stream->delay ? sourceD : stream->delay;
  }
  foreach_(Offset* stream, task->sinks)
    cout<<"sink node: "<<stream->getName()<<" delay: "<<stream->delay<<endl;

  //TODO: we hack here since we assume each function only has 1 output 
  //calculte delay for the sink node 
  foreach_(Offset* stream, task->sinks)
  {
    int Delay = sourceD - stream->delay; 
    cout<<"output delay: "<< Delay <<endl;
    string outputname = stream->getName();
    list<Node*> outputs = task->getNeighbours();
    foreach_(Node* node, outputs)
    {
      //inside each following function
      cout<<"output node: "<<node->getName()<<endl;
      DfeTask *dfeNode = dynamic_cast<DfeTask*>(node);
      //check the matched input
      string matchedname = MatchName(task, dfeNode);

      //get the corresponding param in the function
      string paramName = dfeNode->getCorrespondingKernelParam(matchedname);
      cout<<"matched name: "<<matchedname<<" corespond to "<<paramName<<endl;
      foreach_(Offset* stream, dfeNode->streams)
        if(stream->getName() == paramName) stream->setDelay(Delay);

      foreach_(Offset* stream, dfeNode->streams)
        cout<<"stream "<<stream->getName()<<" delay "<<stream->getDelay()<<endl;

      //check whether the neighbours are the same node?
      //clean the code!
    }
  }
}

void IFEVisitor::ATAPLevel(){

  //traverse the graph with topological sort
  DfeTopSortVisitor topsortVisitor(dfg);
  topsortVisitor.traverse();


  foreach_(Node* node, topsortVisitor.getSorted())
  {
    DfeTask *dfeNode = dynamic_cast<DfeTask*>(node);
    cout<<"node: "<<node->getName()<<" idle cycles: "<< dfeNode->idle<<endl;
    atapTasks.push_back(dfeNode); 
  }
}

struct myclass {
    bool operator() (DfeTask* t1, DfeTask* t2) { return (t1->idle < t2->idle);}
} mysort;

void IFEVisitor::CombineTasks(){

  //first sort the tasks in dfg based on idle cycles
  std::sort(atapTasks.begin(), atapTasks.end(), mysort);
  foreach_(DfeTask* task, atapTasks)
    cout<<"node: "<<task->getName()<<" idle cycles: "<< task->idle<<endl;

  //check the number of levels in the graph
  int curLevel=-1;
  int level   = 0;
  foreach_(DfeTask* task, atapTasks)
    if(curLevel != task->idle) {
      curLevel = task->idle;
      level++; 
    }
  setLevelNum(level);




  //how to store the information
  std::vector<Segment*> levels;//to sotre segments at different levels
  //Segment* levels = new Segment[4];

  levels.resize(getLevelNum());
  vector<Segment*>::iterator it = levels.begin();

  curLevel=-1;
  foreach_(DfeTask* task, atapTasks)
  {
     if(curLevel == task->idle)
      (*it)->addTask(task);//combine nodes at the same level
     else 
     {
       if(curLevel != -1)
         it++;
       (*it) = new Segment();
       (*it)->addTask(task);
       curLevel = task->idle;
     } 
  }

  int i=0;
  foreach_(Segment* seg, levels)
  {
    cout<<"level "<<i++<<endl;
    foreach_(DfeTask* task, seg->getTasks())
      cout<<" node "<<task->getName()<<endl;
  }
}

void IFEVisitor::CombineSegments(){
}

void IFEVisitor::OptimiseConfigurations(){
}

void IFEVisitor::GenerateSolutions(){
}

