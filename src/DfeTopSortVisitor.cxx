#include "DfeTopSortVisitor.hxx"

#include <fstream>

void DfeTopSortVisitor::visit(Node *n) {

  DfeTask *task = (DfeTask *)n;
  cout<<"     visiting kernel: "<<task->getName()<<endl;

  if (task->getName() == "sink" || task->getName() == "source")
    return;

  Kernel* k = task->getKernel();
#if DEBUG
  foreach_(Offset* stream, task->streams)
    cout<<"stream: "<<stream->getName()<<endl;
#endif
  
  //in streams of the task, find the sink node(s)
  list<std::string> sinks = k->getOutputNames();
  foreach_(std::string name, sinks)
    task->sinks.push_back(FindSink(task, name));

  //steams asides sink nodes are source nodes
  FindSource(task);

  //calculate the delay for the sink node(s)
  //as the largetst difference between input node offsets and output
  //node offsets
  cout<<"     updatng kernel delay based on delay of each stream:"<<endl;
  int sourceD=0;
  int idle=0;
  cout<<setw(15)<<"     source stream: "<<setw(15)<<"internaldelay"<<setw(15)<<"inputdelay"<<endl;
  foreach_(Offset* stream, task->sources)
  {
    cout<<setw(15)<<stream->getName()<<setw(15)<<stream->internaldelay<<setw(15)<<stream->inputdelay<<endl;
    sourceD = sourceD > (stream->internaldelay + stream->inputdelay) ? sourceD : 
                                                                       stream->internaldelay + stream->inputdelay;
    idle= idle > stream->inputdelay? idle : stream->inputdelay;
  }

  //now update the output delay
  cout<<"     updating delay of streams connected to this kernel:"<<endl;
  foreach_(Offset* stream, task->sinks)
  {
    int Delay = sourceD;// - stream->internaldelay;
    //now scan all the output nodes
    foreach_(Node* node, task->getNeighbours())
    {
      DfeTask *dfeNode = dynamic_cast<DfeTask*>(node);
      //get the matched input of the output node, and update delay
      string matchedname = MatchName(task, dfeNode);
      string paramName = dfeNode->getCorrespondingKernelParam(matchedname);
      cout<<"     seaching kernel "<<dfeNode->getName();
      cout<<" matched name: "<<matchedname<<" corespond to "<<paramName<<endl;

      foreach_(Offset* stream, dfeNode->streams)
        if(stream->getName() == paramName) {
          stream->setDelay(Delay);         
          D(cout<<"stream "<<stream->getName()<<" delay "<<stream->getDelay()<<endl;)
        }          
    }
  }

  //now calculate the idle cycles
  task-> idle = idle;
  cout<<"     ATAP level (idle cycles): "<<task->idle<<endl<<endl;
  sleep(1);
}

string DfeTopSortVisitor::MatchName(DfeTask *root, DfeTask* branch){

  foreach_(string inputname, branch->getInputs())
    foreach_(string outputname, root->getOutputs())
    if(inputname == outputname) return inputname;

  return NULL;
}


Offset* DfeTopSortVisitor::FindSink(DfeTask* task, string name){
  foreach_(Offset* stream, task->streams)
  {
    if(stream->getName() == name) 
    {
      D(cout<<"sink: "<<name<<endl);
      return stream;
    }
  }
  return NULL;
}

void DfeTopSortVisitor::FindSource(DfeTask* task){
  //steams asides sink nodes are source nodes
  foreach_(Offset* stream, task->streams)
  {
    string name = stream->getName();
    bool tag = false;
    foreach_(Offset* sink, task->sinks)
      if(sink->getName() == name ) 
        tag = true;
    if(tag == false)
    {
      task->sources.push_back(stream);
      D(cout<<"source: "<<name<<endl;)
    }
  }
}



void DfeTopSortVisitor::beforeVisit() {
}

void DfeTopSortVisitor::afterVisit() {
}
