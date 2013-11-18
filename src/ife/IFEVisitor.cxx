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
  this->parnum =0;
}

void IFEVisitor::ExtractProperties(){

  //each function node goes through HLA to extract function properties
  foreach_(Node* node, dfg->getNodes()) {
    //DfeTask *task = dynamic_cast<DfeTask*>(node);
    DfeTask *task = (DfeTask *)node;
    if (task->getName() == "sink" || task->getName() == "source")
      continue;

    Kernel* k = task->getKernel();
    cout<<"     visiting kernel: "<<k->getName()<<endl;
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
    
    task->internaldelay=hlaVisitor.getinternaldelay();
    task->inputdelay   =hlaVisitor.getinputdelay();

    task->ds        = hlaVisitor.getDataSize();
    task->frequency = hlaVisitor.getfrequency();
    sleep(2);
    cout<<endl;
  }
}

Offset* IFEVisitor::FindSink(DfeTask* task, string name){
  foreach_(Offset* stream, task->streams)
  {
    if(stream->getName() == name) 
    {
      cout<<"sink: "<<name<<endl;
      return stream;
    }
  }
  return NULL;
}

void IFEVisitor::FindSource(DfeTask* task){
  //steams asides sink nodes are source nodes
  foreach_(Offset* sink, task->sinks)
  {
    string name = sink->getName();
    foreach_(Offset* stream, task->streams)
      if(stream->getName() != name ) 
      {
        task->sources.push_back(stream);
        cout<<"source: "<<name<<endl;
      }
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
    D(cout<<"source node: "<<stream->getName()<<" delay: "<<stream->internaldelay<<endl;)
    sourceD = sourceD > stream->internaldelay ? sourceD : stream->internaldelay;
  }
  foreach_(Offset* stream, task->sinks)
    D(cout<<"sink node: "<<stream->getName()<<" delay: "<<stream->internaldelay<<endl;)

  //TODO: we hack here since we assume each function only has 1 output
  //calculte delay for the sink node
  foreach_(Offset* stream, task->sinks)
  {
    int Delay = sourceD - stream->internaldelay;
    D(cout<<"output delay: "<< Delay <<endl;)
    string outputname = stream->getName();
    list<Node*> outputs = task->getNeighbours();
    foreach_(Node* node, outputs)
    {
      //inside each following function
      D(cout<<"output node: "<<node->getName()<<endl;)
      DfeTask *dfeNode = dynamic_cast<DfeTask*>(node);
      //check the matched input
      string matchedname = MatchName(task, dfeNode);

      //get the corresponding param in the function
      string paramName = dfeNode->getCorrespondingKernelParam(matchedname);
      D(cout<<"matched name: "<<matchedname<<" corespond to "<<paramName<<endl;)
      foreach_(Offset* stream, dfeNode->streams)
        if(stream->getName() == paramName) stream->setDelay(Delay);

#if DEBUG
      foreach_(Offset* stream, dfeNode->streams)
        cout<<"stream "<<stream->getName()<<" delay "<<stream->getDelay()<<endl;
#endif
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
    D(cout<<"node: "<<node->getName()<<" idle cycles: "<< dfeNode->idle<<endl;)
    atapTasks.push_back(dfeNode);
  }
}

struct myclass {
    bool operator() (DfeTask* t1, DfeTask* t2) { return (t1->idle < t2->idle);}
} mysort;

void IFEVisitor::CombineTasks(){

  //first sort the tasks in dfg based on idle cycles
  std::sort(atapTasks.begin(), atapTasks.end(), mysort);

#if DEBUG
  foreach_(DfeTask* task, atapTasks)
    D(cout<<"node: "<<task->getName()<<" idle cycles: "<< task->idle<<endl;)
#endif

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
       stringstream ssidle;
       ssidle<< (task->idle);
       string sslevel = ssidle.str();

       levels.push_back(new Segment(sslevel));
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
    cout<<"     level "<<seg->getName()<<endl;
    foreach_(DfeTask* task, seg->getTasks())
      cout<<"     "<<task->getName()<<endl;
    cout<<endl;
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


  foreach_(Configuration* con, configurations)
  {
    cout<<"     configuration "<<con->getName()<<endl;
    foreach_(Segment* seg, con->getConfiguration())
      foreach_(DfeTask* task, seg->getTasks())
      {
        cout<<"     "<<task->getName()<<endl;
      }
    cout<<endl;
  }
}

bool IFEVisitor::FindOverlappedKernel(DfeTask* task){
  foreach_(DfeTask* branch, seenTasks)
    if(branch->getName() != task->getName() && 
        (branch->getKernel())->getName() == (task->getKernel())->getName())
    {
      cout<<"     overlapped task: "<<task->getName()<<endl;
      return true;
    }
  return false;
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
              D(cout<<"mathed name: "<<outputname<<endl;)
              find++;
            }
  return find;
}

void IFEVisitor::OptimiseConfigurations(){
  
  //aggregrate the function properties, LUTs, FFs, BRAMs, DSPs
  cout<<"     accumulating extracted kernel properties, for non-overlapped kernels"<<endl;
  foreach_(Configuration* con, configurations)
  {
    seenTasks.clear();
    foreach_(Segment* seg, con->getConfiguration())
    {
      foreach_(DfeTask* task, seg->getTasks())
      {
        if(!FindOverlappedKernel(task))
        {
          con->LUTs += task->LUTs;
          con->FFs  += task->FFs;
          con->DSPs += task->DSPs;
          con->BRAMs+= task->BRAMs;
        }
        seenTasks.push_back(task);
      }
    }
  }
  
  //aggregrate the bandwidth
  //TODO: there are kernels sharing the same input data, need to process that
  //TODO: for that case, also need to think how to merge the onchipmemory 
  cout<<"     aggregrating off-chip memory bandwidth for each configuration"<<endl;
  foreach_(Configuration* con, configurations)
    foreach_(Segment* seg, con->getConfiguration())
      foreach_(DfeTask* task, seg->getTasks())
      {
        Kernel* kbuf = task->getKernel();
        DataFlowGraph* dbuf = kbuf->getDataFlowGraph(); 
        foreach_(Offset* stream, dbuf->streams)
        {
          D(cout<<"con "<<con->getName()<<"stream "<<stream->getName()<<" "<<endl;)
          con->bandwidth += stream->bandwidth;
          int find = FindOutput(task, con, stream->getName());
          if(find>0)
          {
            D(cout<<"find "<<find<<" connected for con "<<con->getName()<<" with stream "<<stream->getName()<<endl;)
            con->bandwidth -= (((double) find) +1)*stream->bandwidth;
          }
          D(cout<<"band: "<<con->bandwidth<<endl;)
        }
      }
  
  foreach_(Configuration* con, configurations)
  {
    cout<<"     configuration "<<con->getName()<<endl;
    cout<<"     banwidth: "<<con->bandwidth<<endl;
  }

  //calculate the parallelism 
  //TODO: explore DSP factor and precision
  cout<<"     optimising each configuration to achieve maximum configuration"<<endl;
  foreach_(Configuration* con, configurations)
  {
    con->P = con->P < floor((con->Abw) / (con->bandwidth)) ? con->P : floor((con->Abw) / (con->bandwidth));
    con->P = con->P < floor((con->Al-con->Il)/(con->LUTs)) ? con->P : floor((con->Al-con->Il)/(con->LUTs));
    con->P = con->P < floor((con->Af-con->If)/(con->FFs))  ? con->P : floor((con->Af-con->If)/(con->FFs));
    con->P = con->P < floor((con->Ad-con->Id)/(con->DSPs)) ? con->P : floor((con->Ad-con->Id)/(con->DSPs));
    //con->P = con->P < ceil((con->Ab-con->Ib)/(con->BRAMs))? con->P : ceil((con->Ab-con->Ib)/(con->BRAMs));
  }
  
  //aggregrate the resource consumption for infrasttructure
  cout<<"     updating resource consumption based on infrastructure resource and optimised parallelism"<<endl;
  foreach_(Configuration* con, configurations)
  {
    con->LUTs      = con->Il + con->LUTs *con->P  ;
    con->FFs       = con->If + con->FFs  *con->P ;
    con->DSPs      = con->Id + con->DSPs *con->P ;
    con->BRAMs     = con->Ib + con->BRAMs;
    con->bandwidth = con->bandwidth * con->P ;
  }                                   

  foreach_(Configuration* con, configurations)
  {
    cout<<"     configuration "<<con->getName()<<endl;
    cout<<"     banwidth: "<<con->bandwidth<<endl;
    cout<<"     LUTs  "<<con->LUTs<<endl;
    cout<<"     FFs   "<<con->FFs<<endl;
    cout<<"     DSPs  "<<con->DSPs<<endl;
    cout<<"     BRAMs "<<con->BRAMs<<endl;
    cout<<"     Parallelism "<<con->P<<endl;
    cout<<endl;
  }
  
  //TODO: iterate the pass to find the optimal DSP factor and precision
  //TODO: data blocking for multi-dimenion offsets
}

void IFEVisitor::GenerateSolutions(){

  //RULE3: combine configurations based on levels and configuration size
  Partition* parbuf;
  foreach_(int level, levelNums)
  {
    parbuf = new Partition("buf");
    foreach_(Configuration* con, configurations)
    if(level == con->level)
      parbuf->addConfiguration(con);
    configurationGraph.push_back(parbuf);
  }

#if DEBUG
  int i=0;
  foreach_(Partition* par, configurationGraph)
  {
    D(cout<<"levels: "<<i++<<endl;)
    foreach_(Configuration* con, par->getPartition())
      D(cout<<"configuration "<<" level:"<<con->level<<" size:"<<con->getConfiguration().size()<<endl;)
  }
#endif

  std::vector<Partition*>::iterator it = configurationGraph.begin();

  // XXX Apparently the configuration graph can be empty which breaks
  // the following code
  if (it == configurationGraph.end())
    return;

  cout<<"     combining configuration into valid run-time solutions, based the configuration graph"<<endl;
  foreach_(Configuration* con, (*it)->getPartition())
  {
    stringstream parsize;
    parsize<< (parnum+1);
    string size = parsize.str();

    parbuf = new Partition(size);
    parnum++;
    parbuf->addConfiguration(con);
    int nextlevel = con->getConfiguration().size();

    if(nextlevel == levelNums.size())
      partitions.push_back(parbuf);
    else
      FindPartition(nextlevel, parbuf);
  }
  sleep(2);
  
  foreach_(Partition* par, partitions)
  {
    cout<<"     partition( run-time solution) "<<par->getName()<<": "<<endl;
    foreach_(Configuration* con, par->getPartition())
      cout<<"     configuration "<<con->getName()<<endl;
    cout<<endl;
  }
}

bool IFEVisitor::FindLevel(Segment* level, Configuration* con)
{
  foreach_(Segment* seg, con->getConfiguration())
      if(seg->getName() == level->getName()) 
        return true; 
  return false;
}


void IFEVisitor::EvaluateSolutions(){

  cout<<"     evaluating execution time of generated partitions"<<endl;  
  foreach_(Partition* par, partitions)
  {
    cout<<"     partition: "<<par->getName()<<endl;
    double exeTime = 0;
    double levTime = 0;
    double conTime = 0;

    std::vector<Configuration*> configurations = par->getPartition();
    std::vector<Configuration*>::iterator curCon = configurations.begin();///iterate
    exeTime += (*curCon)->getReconfigurationTime();//initial configuration time
    foreach_(Segment* seg,  levels)//evalaute level by level
    {
      levTime = 0;
      //within each level, calculate the execution time for each task
      cout<<"     executing level: "<<seg->getName()<<" at "<<exeTime<<"s";
      cout<<" with configuration"<<(*curCon)->getName()<<endl;

      double timebuf; 
      foreach_(DfeTask* task, seg->getTasks())
      {
        //timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)(*curCon)->P); 
        timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)((*curCon)->P) / 6); 
        levTime = levTime > timebuf ? levTime : timebuf; 
      }
      if(!FindLevel(seg, *curCon))//cannot find level in this configuration
      {
       (*curCon)->setexecutionTime(conTime);
       cout<<"con time "<<conTime<<endl;
       D(cout<<"execution time for "<<(*curCon)->getName()<<" "<<(*curCon)->getexecutionTime()<<endl;) 
       curCon++;
       foreach_(DfeTask* task, seg->getTasks())
       {
         //timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)(*curCon)->P); 
         timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)((*curCon)->P) / 6); 
         levTime = levTime > timebuf ? levTime : timebuf; 
       }
       conTime = levTime;
       exeTime +=levTime;
       cout<<"     reconfiguration triggerred, current configuration reconfigured to be configuration ";
       cout<<(*curCon)->getName()<<endl;
       exeTime += (*curCon)->getReconfigurationTime(); 
      }
      else//this level is executed in this configuration 
      {
        conTime += levTime;
        exeTime += levTime;//accumulate execution time for each level, as they cannot run in parallel
      }
      cout<<"     finish with "<<exeTime<<"s"<<endl; 
    }
    par->setexecutionTime(exeTime);
    (*curCon)->setexecutionTime(conTime);
    D(cout<<"execution time for "<<(*curCon)->getName()<<" "<<(*curCon)->getexecutionTime()<<endl;) 
    cout<<endl;
  }
#if DEBUG
  foreach_(Partition* par, partitions)
    cout<<par->getexecutionTime()<<endl;
#endif
}

void IFEVisitor::FindPartition(int start, Partition* par){

  Partition* parbuf;
  std::vector<Partition*>::iterator it = configurationGraph.begin();

  foreach_(Configuration* con, ((*(it+start))->getPartition()))
  {
    stringstream parsize;
    parsize<< (parnum+1);
    string size = parsize.str();
    parbuf = new Partition(size);
    //parnum++;
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


bool IFEVisitor::seenConfiguration(Configuration* con) {

  foreach_ (Configuration* seen, seenConfigurations) {
    if (seen->getName() == con->getName())
      return true;
  }
  return false;
}
