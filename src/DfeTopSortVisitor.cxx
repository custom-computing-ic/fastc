#include "DfeTopSortVisitor.hxx"

#include <fstream>

void DfeTopSortVisitor::visit(Node *n) {

  DfeTask *task = (DfeTask *)n;
  cout<<"visiting node: "<<task->getName()<<endl;

  if (task->getName() == "sink" || task->getName() == "source")
    return;

  Kernel* k = task->getKernel();
  
  //in streams of the task, find the sink node(s)
  list<std::string> sinks = k->getOutputNames();
  foreach_(std::string name, sinks)
    task->sinks.push_back(FindSink(task, name));

  //steams asides sink nodes are source nodes
  FindSource(task);

  //calculate the delay for the sink node(s)
  //as the largetst difference between input node offsets and output
  //node offsets
  int sourceD=0;
  foreach_(Offset* stream, task->sources)
  {
    cout<<"source node: " <<stream->getName()<<" internal delay: "<<stream->internaldelay;
    cout<<" input delay: "<<stream->inputdelay<<endl;
    sourceD = sourceD > (stream->internaldelay + stream->inputdelay) ? 
                        sourceD : (stream->internaldelay + stream->inputdelay);
  }

  //now update the output delay
  foreach_(Offset* stream, task->sinks)
  {
    int Delay = sourceD - stream->internaldelay;
    //now scan all the output nodes
    foreach_(Node* node, task->getNeighbours())
    {
      DfeTask *dfeNode = dynamic_cast<DfeTask*>(node);
      //get the matched input of the output node, and update delay
      string matchedname = MatchName(task, dfeNode);
      string paramName = dfeNode->getCorrespondingKernelParam(matchedname);
      cout<<"matched name: "<<matchedname<<" corespond to "<<paramName<<endl;

      foreach_(Offset* stream, dfeNode->streams)
        if(stream->getName() == paramName) {
          stream->setDelay(Delay);         
          cout<<"stream "<<stream->getName()<<" delay "<<stream->getDelay()<<endl;
        }          

//    foreach_(Offset* stream, dfeNode->streams)
//      cout<<"stream "<<stream->getName()<<" delay "<<stream->getDelay()<<endl;
    }
  }

  //now calculate the idle cycles
  task-> idle = sourceD;
   
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
    if(stream->getName() == name) return stream;
  }
  return NULL;
}

void DfeTopSortVisitor::FindSource(DfeTask* task){
  //steams asides sink nodes are source nodes
  foreach_(Offset* sink, task->sinks)
  {
    string name = sink->getName();
    foreach_(Offset* stream, task->streams)
      if(stream->getName() != name ) task->sources.push_back(stream);
  }
}






void DfeTopSortVisitor::beforeVisit() {
}

void DfeTopSortVisitor::afterVisit() {
}
