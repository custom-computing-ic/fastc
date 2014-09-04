#include "IFEVisitor.hxx"
#include <sstream>
#include <math.h>
#include <vector>
#include <list>
#include <algorithm>    // std::sort
#include "../highlevel/HLAVisitor.hxx"
#include "../DfeTopSortVisitor.hxx"
#include "../precompiled.hxx"

#include <libxml++/libxml++.h>
#include <iostream>


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
    cout<<"     visiting kernel: \033[1;31m"<<k->getName()<<"\033[0m"<<endl;

//  for(std::map<string,string>::iterator it=(k->ioTypeMap).begin(); it!=(k->ioTypeMap).end(); ++it)
//    std::cout << it->first << " => " << it->second << '\n';

//  cout<<(k->ioTypeMap)["p"]<<endl;
//  //std::string xx = "hwFloat8, 24";
//  (k->ioTypeMap)["p"] = "hwFloat(8, 24)";
//  //boost::regex *TYPE = new boost::regex("([a-zA-Z]*)([0-9]*)(,\s*)(([0-9]*))?");
//  boost::regex* r = new boost::regex("[a-zA-Z]*\\(([0-9]*),\\s*([0-9]*)\\)");
//  boost::cmatch group;
//  if (boost::regex_match((k->ioTypeMap)["p"].c_str(), group, *r))
//  //if (boost::regex_match((xx.c_str(), group, *TYPE)))
//    cout<<group[0]<<" "<<group[1]<<endl;


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
    //sleep(1);
    cout<<endl;
  }
}

template <typename T>
std::string to_string(T const& value) {
  stringstream sstr;
  sstr << value;
  return sstr.str();
}


void IFEVisitor::UpdateXML(){

  std::string filepath, filepath_out;
  filepath     = "faster_initial.xml";
  filepath_out = "faster_hla.xml";
  
  XMLNode xMainNode=XMLNode::openFileHelper("Faster.xml","Faster_XML");

  XMLNode xNode=xMainNode.getChildNode("architecture").getChildNode("system");
  XMLNode pNode = xNode.addChild("processing_elements");
  XMLNode mNode = xNode.addChild("memory_elements");

  try
  {
    xmlpp::DomParser parser;
    parser.set_substitute_entities(); //We just want the text to be resolved/unescaped automatically.
    parser.parse_file(filepath);
    if(parser)
    { 
      xmlpp::Document* document = parser.get_document();
      xmlpp::Node* root_node = document->get_root_node(); //deleted by DomParser.

      xmlpp::Node::NodeList task_list = root_node->get_children();
      xmlpp::Node* library;
      xmlpp::Element* partition;
      for(xmlpp::Node::NodeList::iterator p_it = task_list.begin(); p_it != task_list.end(); p_it++)
      //iterate through the first-level nodes, looking for partitions and libraries
      { 
        xmlpp::Node* t_node = dynamic_cast<xmlpp::Node*>(*p_it);
        if(t_node->get_name()=="library")
           library = dynamic_cast<xmlpp::Node*>(*p_it);
        else if(t_node->get_name()=="partitions")
        {
          partition = t_node->add_child("partition");
          partition->set_attribute("id", "partition0");

          foreach_(Node* node, dfg->getNodes()) {
            DfeTask *task = (DfeTask *)node;
            if (task->getName() == "sink" || task->getName() == "source")
              continue;

            Kernel* k = task->getKernel();
            cout<<"kernel: \033[1;31m"<<k->getName()<<"\033[0m"<<endl;

            task->bandwidth = 0;
            DataFlowGraph* d = k->getDataFlowGraph();
            foreach_(Offset* stream, d->streams)
            {
              cout<<"stream: "<<stream->getName()<<endl;
              task->bandwidth += stream->bandwidth;
              task->precision =  stream->precision[0] + stream->precision[1];
            }

            cout<<"BRAMs:     "<<task->BRAMs     <<endl;
            cout<<"LUTs:      "<<task->LUTs      <<endl;
            cout<<"FFs:       "<<task->FFs       <<endl;
            cout<<"DSPs:      "<<task->DSPs      <<endl;
            cout<<"bandwidth: "<<task->bandwidth <<endl;

            xmlpp::Element* xml_task = partition->add_child("task");
            xml_task->set_attribute("id", task->getName().c_str());

            xmlpp::Element* spec = xml_task->add_child("spec");
            spec->set_attribute("name","function");

            //software implementation
            std::string arm = "ARM_";
            xmlpp::Element* imp  = library->add_child("implementation");
            imp->set_attribute("id",arm+(task->getName()).c_str());

                            xml_task = imp->add_child("task");
            xml_task->set_attribute("id", task->getName().c_str());
            
            xmlpp::Element* com  = imp->add_child("component");
            com->set_attribute("type", "SW");

                            spec = imp->add_child("spec");
            spec->set_attribute("name", "execution_time");
            spec->set_attribute("value",to_string(task->ds * 12).c_str());
            spec->set_attribute("unit", "cycles");

            //hardware implementation
            std::string max = "MAX_";
                            imp  = library->add_child("implementation");
            imp->set_attribute("id",max+(task->getName()).c_str());

                            xml_task = imp->add_child("task");
            xml_task->set_attribute("id", task->getName().c_str());

                            com  = imp->add_child("component");
            com->set_attribute("type", "HW");

                            spec = imp->add_child("spec");
            spec->set_attribute("name", "execution_time");
            spec->set_attribute("value",to_string(task->ds).c_str());
            spec->set_attribute("unit", "cycles");
                            
            spec = imp->add_child("spec");
            spec->set_attribute("name", "area");
            spec->set_attribute("value",to_string(task->LUTs).c_str());
            
            spec = imp->add_child("spec");
            spec->set_attribute("name", "FFs");
            spec->set_attribute("value",to_string(task->FFs).c_str());
            
            spec = imp->add_child("spec");
            spec->set_attribute("name", "DSPs");
            spec->set_attribute("value",to_string(task->DSPs).c_str());
            
            spec = imp->add_child("spec");
            spec->set_attribute("name", "BRAMs");
            spec->set_attribute("value",to_string(task->BRAMs).c_str());
            
            spec = imp->add_child("spec");
            spec->set_attribute("name", "memory_bandwidth");
            spec->set_attribute("value",(char*)to_string(task->bandwidth).c_str());
            spec->set_attribute("unit", "MB/s");
            
            double data_size =(task->ds) * (task->precision) / (1024 * 1024 * 8);
            spec = imp->add_child("spec");
            spec->set_attribute("name", "data_size");
            spec->set_attribute("value",(char*)to_string(data_size).c_str());
            spec->set_attribute("unit", "MB");
            
            spec = imp->add_child("spec");
            spec->set_attribute("name", "frequency");
            spec->set_attribute("value",(char*)to_string(task->frequency).c_str());
            spec->set_attribute("unit", "MHz");
            
            foreach_(string stream, task->getOutputs())
            {
              spec = imp->add_child("spec");
              spec->set_attribute("name", "port");
              spec->set_attribute("id", stream.c_str());
              spec->set_attribute("size", (char *) to_string(task->precision).c_str());
              spec->set_attribute("direction", "OUT");
            } 

            foreach_(string stream, task->getInputs())
            {
              spec = imp->add_child("spec");
              spec->set_attribute("name", "port");
              spec->set_attribute("id", stream.c_str());
              spec->set_attribute("size", (char *) to_string(task->precision).c_str());
              spec->set_attribute("direction", "IN");
            }

          }
        }
      }
      document->write_to_file(filepath_out);
    }
  }
  catch(const std::exception& ex)
  {
    std::cout << "Exception caught: " << ex.what() << std::endl;
  }



  /*
  foreach_(Node* node, dfg->getNodes()) {
    DfeTask *task = (DfeTask *)node;
    if (task->getName() == "sink" || task->getName() == "source")
      continue;

    Kernel* k = task->getKernel();
    cout<<"kernel: \033[1;31m"<<k->getName()<<"\033[0m"<<endl;

    task->bandwidth = 0;
    DataFlowGraph* d = k->getDataFlowGraph();
    foreach_(Offset* stream, d->streams)
    {
      cout<<"stream: "<<stream->getName()<<endl;
      task->bandwidth += stream->bandwidth;
      task->precision =  stream->precision[0] + stream->precision[1];
    }

    cout<<"BRAMs:     "<<task->BRAMs     <<endl;
    cout<<"LUTs:      "<<task->LUTs      <<endl;
    cout<<"FFs:       "<<task->FFs       <<endl;
    cout<<"DSPs:      "<<task->DSPs      <<endl;
    cout<<"bandwidth: "<<task->bandwidth <<endl;


    XMLNode rNode = pNode.addChild("reconfigurable_area");
    rNode.addAttribute("id", (char*)task->getName().c_str());

    cout<<task->LUTs<<endl;
    cout<<to_string(task->LUTs)<<endl;

    XMLNode sNode;
    sNode = rNode.addChild("specs");
    sNode.addAttribute("LUTs", (char *)to_string(task->LUTs).c_str());
    sNode = rNode.addChild("specs");
    sNode.addAttribute("FFs",  (char *)to_string(task->FFs).c_str());

    sNode = rNode.addChild("specs");
    sNode.addAttribute("DSPs", (char *)to_string(task->DSPs).c_str());

    sNode = rNode.addChild("specs");
    sNode.addAttribute("BRAMs",(char *)to_string(task->BRAMs).c_str());

    sNode = rNode.addChild("specs");
    sNode.addAttribute("execution time",(char *)to_string(task->ds).c_str());
    sNode.addAttribute("unit", "cycles");

    XMLNode bNode;
    bNode = mNode.addChild("memory");
    bNode.addAttribute("id",(char*)task->getName().c_str());
    bNode.addAttribute("type","DRAM / BRAM");
    sNode = bNode.addChild("specs");
    sNode.addAttribute("data rate",(char*)to_string(task->bandwidth).c_str());
    sNode.addAttribute("unit","MB");
    sNode = bNode.addChild("specs");
    double data_size =(task->ds) * (task->precision) / (1024 * 1024 * 8);
    cout<<task->precision<<endl;
    cout<<task->ds<<endl;
    cout<<data_size<<endl;

    sNode.addAttribute("size",     (char*)to_string(data_size).c_str());
    sNode.addAttribute("unit","MB");
    sNode = bNode.addChild("specs");
    sNode.addAttribute("frequency",(char*)to_string(task->frequency).c_str());
    sNode.addAttribute("unit","MHz");

    sNode = bNode.addChild("interface");
    XMLNode pNode;
    foreach_(string stream, task->getOutputs())
    {
      pNode = sNode.addChild("port");
      pNode.addAttribute("id", stream.c_str());
      pNode.addAttribute("size", (char *) to_string(task->precision).c_str());
      pNode.addAttribute("direction", "OUT");
    }

    foreach_(string stream, task->getInputs())
    {
      pNode = sNode.addChild("port");
      pNode.addAttribute("id", stream.c_str());
      pNode.addAttribute("size", (char *) to_string(task->precision).c_str());
      pNode.addAttribute("direction", "IN");
    }
  }
  */
  //keep the current write opertions in case they are needed 

  xMainNode.writeToFile("HLA.xml");
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

  D(foreach_(Offset* stream, task->sinks))
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
       if(cap != (int)levels.capacity()) //expanding
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

  foreach_(Segment* seg, levels)
  {
    cout<<"     \033[1;31mlevel "<<seg->getName()<<"\033[0m"<<endl;
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
    cout<<"     \033[1;31mconfiguration "<<con->getName()<<"\033[0m"<<endl;
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
    if(task->getCorrespondingKernelParam(outputname) == name){//matching the searched streamed
      foreach_(Segment* seg, con->getConfiguration())
        foreach_(DfeTask* branch, seg->getTasks())
          foreach_(string inputname, branch->getInputs())
            if(outputname == inputname)
            {
              D(cout<<"mathed name: "<<outputname<<endl;)
              find++;
            }
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
    cout<<"     \033[1;31mconfiguration "<<con->getName()<<"\033[0m"<<endl;
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
    size_t nextlevel = con->getConfiguration().size();

    if(nextlevel == levelNums.size())
      partitions.push_back(parbuf);
    else
      FindPartition(nextlevel, parbuf);
  }
  //sleep(2);

  foreach_(Partition* par, partitions)
  {
    cout<<"     \033[1;31mpartition( run-time solution) "<<par->getName()<<": "<<"\033[0m"<<endl;
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
      cout<<" with \033[1;31mconfiguration"<<(*curCon)->getName()<<"\033[0m"<<endl;

      double timebuf;
      foreach_(DfeTask* task, seg->getTasks())
      {
        //timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)(*curCon)->P);
        timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)((*curCon)->P) / 8);
        levTime = levTime > timebuf ? levTime : timebuf;
      }
      if(!FindLevel(seg, *curCon))//cannot find level in this configuration
      {
       (*curCon)->setexecutionTime(conTime);
       D(cout<<"execution time for "<<(*curCon)->getName()<<" "<<(*curCon)->getexecutionTime()<<endl;)
       curCon++;
       foreach_(DfeTask* task, seg->getTasks())
       {
         //timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)(*curCon)->P);
         timebuf = task->ds * 1000 / (task->frequency * 1000000 * (double)((*curCon)->P) / 8);
         levTime = levTime > timebuf ? levTime : timebuf;
       }
       conTime = levTime;
       exeTime +=levTime;
       cout<<"     \033[1;31mreconfiguration triggerred\033[0m, current configuration reconfigured to be \033[1;31mconfiguration ";
       cout<<(*curCon)->getName()<<"\033[0m"<<endl;
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
    size_t nextlevel = (start + con->getConfiguration().size());

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
