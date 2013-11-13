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
    cout<<"visiting kernel: "<<k->getName()<<endl;
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
    cout<<"source node: "<<stream->getName()<<" delay: "<<stream->internaldelay<<endl;
    sourceD = sourceD > stream->internaldelay ? sourceD : stream->internaldelay;
  }
  foreach_(Offset* stream, task->sinks)
    cout<<"sink node: "<<stream->getName()<<" delay: "<<stream->internaldelay<<endl;

  //TODO: we hack here since we assume each function only has 1 output
  //calculte delay for the sink node
  foreach_(Offset* stream, task->sinks)
  {
    int Delay = sourceD - stream->internaldelay;
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
//int curLevel=-1;
//int level   = 0;
//foreach_(DfeTask* task, atapTasks)
//  if(curLevel != task->idle) {
//    curLevel = task->idle;
//    level++;
//  }
//setLevelNum(level);

  //resize the levels based on levelNum;
  //levels.resize(getLevelNum());
  //levels.reserve(100);
  vector<Segment*>::iterator it = levels.begin();

  int curLevel=-1;
  int cap, offset;
  offset = it - levels.begin();
  cap    = 0;
  //RULE1: functions same idle cycles are compressed into the same level
  foreach_(DfeTask* task, atapTasks)
  {
     if(curLevel == task->idle)
      (*it)->addTask(task);//combine nodes at the same level
     else
     {
       levels.push_back(new Segment());
       if(cap != levels.capacity()) //expanding
       {
         it = levels.begin() + offset;
         cap= levels.capacity();
       }
       if(cap != 1)
       {
         it++;
         offset++;
       }
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


  int levelnum =0;
  //RULE2: segments are combined in this order to protect data dependency
  Configuration* conbuf = new Configuration("0-0");
  Configuration* prebuf = new Configuration("0-0");
  for(vector<Segment*>::iterator it = levels.begin(); it!=levels.end(); it++)
  {
    //level name
    stringstream sslevel;
    sslevel<< (levelnum+1);
    string level = sslevel.str();

    int consize =1;
    for(vector<Segment*>::iterator jt = it; jt!=levels.end(); jt++)
    {
      //size name
      stringstream sssize;
      sssize<< (consize);
      string size = sssize.str();

      conbuf = new Configuration(level+"-"+size);
      if(jt!=it)
        *conbuf = *prebuf;
      else
        conbuf->level = levelnum;
      conbuf->setName(level+"-"+size);
      prebuf = conbuf;
      conbuf->addSegment(*jt);
      configurations.push_back(conbuf);

      consize++;
    }

    levelNums.push_back(levelnum);
    levelnum++;
  }

#if DEBUG
  foreach_(Configuration* con, configurations)
  {
    cout<<"configuration "<<" level:"<<con->level<<" size:"<<con->getConfiguration().size()<<endl;
    foreach_(Segment* seg, con->getConfiguration())
      foreach_(DfeTask* task, seg->getTasks())
        cout<<" Task "<<task->getName()<<endl;
  }
#endif
}

int IFEVisitor::FindOutput(DfeTask* task, Configuration* con, string name)
{
  int find =0;
  foreach_(string outputname, task->getOutputs())
    if(task->getCorrespondingKernelParam(outputname) == name) //matching the searched streamed
      foreach_(Segment* seg, con->getConfiguration())
        foreach_(DfeTask* branch, seg->getTasks())
          foreach_(string inputname, branch->getInputs())
            if(outputname == inputname)
            {
              cout<<"mathed name: "<<outputname<<endl;
              find++;
            }
  return find;
}

void IFEVisitor::OptimiseConfigurations(){
  //aggregrate the function properties, LUTs, FFs, BRAMs, DSPs
  foreach_(Configuration* con, configurations)
  {
    foreach_(Segment* seg, con->getConfiguration())
      foreach_(DfeTask* task, seg->getTasks())
      {
         con->LUTs += task->LUTs;
         con->FFs  += task->FFs;
         con->DSPs += task->DSPs;
         con->BRAMs+= task->BRAMs;
      }
  }

  //aggregrate the bandwidth
  //TODO: there are kernels sharing the same input data, need to process that
  //TODO: for that case, also need to think how to merge the onchipmemory 
  foreach_(Configuration* con, configurations)
    foreach_(Segment* seg, con->getConfiguration())
      foreach_(DfeTask* task, seg->getTasks())
      {
        Kernel* kbuf = task->getKernel();
        DataFlowGraph* dbuf = kbuf->getDataFlowGraph(); 
        foreach_(Offset* stream, dbuf->streams)
        {
          cout<<"con "<<con->getName()<<"stream "<<stream->getName()<<" "<<endl;
          con->bandwidth += stream->bandwidth;
          int find = FindOutput(task, con, stream->getName());
          if(find>0)
          {
            cout<<"find "<<find<<" connected for con "<<con->getName()<<" with stream "<<stream->getName()<<endl;
            con->bandwidth -= (((double) find) +1)*stream->bandwidth;
          }
          cout<<"band: "<<con->bandwidth<<endl;
        }
      }

  foreach_(Configuration* con, configurations)
  {
    cout<<"configuration "<<con->getName()<<endl;
    cout<<"banwidth: "<<con->bandwidth<<endl;
    cout<<"LUTs  "<<con->LUTs<<endl;
    cout<<"FFs   "<<con->FFs<<endl;
    cout<<"DSPs  "<<con->DSPs<<endl;
    cout<<"BRAMs "<<con->BRAMs<<endl;
  }

  //TODO: calculate the parallelism
  //TODO: iterate the pass to find the optimal DSP factor and precision
  //TODO: data blocking for multi-dimenion offsets
}

void IFEVisitor::GenerateSolutions(){

  //RULE3: combine configurations based on levels and configuration size
  Partition* parbuf;
  foreach_(int level, levelNums)
  {
    parbuf = new Partition();
    foreach_(Configuration* con, configurations)
    if(level == con->level)
      parbuf->addConfiguration(con);
    configurationGraph.push_back(parbuf);
  }

#if DEBUG
  int i=0;
  foreach_(Partition* par, configurationGraph)
  {
    cout<<"levels: "<<i++<<endl;
    foreach_(Configuration* con, par->getPartition())
      cout<<"configuration "<<" level:"<<con->level<<" size:"<<con->getConfiguration().size()<<endl;
  }
#endif

  std::vector<Partition*>::iterator it = configurationGraph.begin();

  // XXX Apparently the configuration graph can be empty which breaks
  // the following code
  if (it == configurationGraph.end())
    return;

  foreach_(Configuration* con, (*it)->getPartition())
  {
    parbuf = new Partition();
    parbuf->addConfiguration(con);
    int nextlevel = con->getConfiguration().size();

    if(nextlevel == levelNums.size())
      partitions.push_back(parbuf);
    else
      FindPartition(nextlevel, parbuf);
  }

  foreach_(Partition* par, partitions)
  {
    cout<<"partition: "<<endl;
    foreach_(Configuration* con, par->getPartition())
      cout<<" "<<con->getName()<<endl;
  }
}

void IFEVisitor::FindPartition(int start, Partition* par){

  Partition* parbuf;
  std::vector<Partition*>::iterator it = configurationGraph.begin();

  foreach_(Configuration* con, ((*(it+start))->getPartition()))
  {
    parbuf = new Partition();
    *parbuf = * par;
    parbuf->addConfiguration(con);
    int nextlevel = (start + con->getConfiguration().size());

    if(nextlevel == levelNums.size())
      partitions.push_back(parbuf);
    else
      FindPartition(nextlevel, parbuf);
  }
  return;
}



//    par->addConfiguration(con);
//    seenConfigurations.push_back(con);
//    cout<<"added: "<<con->getName()<<endl;

//    std::vector<int>::iterator it = levelNums.begin();
//    int offset = start + con->getConfiguration().size();

//    if(offset >= levelNums.size())//reach the end point
//    {
//      cout<<"arrive here, return!"<<endl;
//      seenConfigurations.pop_back();
//      return true;
//    }

//    int targetlevel = *(it + offset);
//    if(FindConfiguration(targetlevel, par))//find a valid partition
//    {
//      seenConfigurations.push_back(con);
//      return true;
//    }
//    else
//    {
//      seenConfigurations.push_back(con);
//      return false;
//    }

bool IFEVisitor::seenConfiguration(Configuration* con) {

  foreach_ (Configuration* seen, seenConfigurations) {
    if (seen->getName() == con->getName())
      return true;
  }
  return false;
}
