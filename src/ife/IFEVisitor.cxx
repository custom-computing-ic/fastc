#include "IFEVisitor.hxx"
#include <sstream>
#include <math.h>     
#include <vector>
#include <list>
#include "../highlevel/HLAVisitor.hxx"


IFEVisitor::IFEVisitor(DfeGraph *dfe){
  this->dfe = dfe;
}

void IFEVisitor::ExtractProperties(){
  
  //each function node goes through HLA to extract function properties
  foreach_(DfeTask* task, dfe->getTasks()) {
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
  //steams excepts sink nodes are source nodes
  foreach_(Offset* sink, task->sinks)
  {
    string name = sink->getName();
    foreach_(Offset* stream, task->streams)
      if(stream->getName() != name ) task->sources.push_back(stream);
  }
}

void IFEVisitor::ATAPLevel(){

  //first build the whole grah based on inputs/outputs
  foreach_(DfeTask* task, dfe->getTasks()) {
    cout<<"Task: "<<task->getName()<<endl;

//  
//  foreach_(Offset* stream, task->streams)
//    cout<<"stream node: "<<stream->getName()<<" delay: "<<stream->delay<<endl;
  
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
      cout<<"output delay: "<<sourceD - stream->delay<<endl;

    list<Node*> outputs = task->getNeighbours();
    foreach_(Node* node, outputs)
      cout<<"output node: "<<node->getName()<<endl;

    //we nned to implement something to match the function input to the
    //actual input, using the order of the arguments

    std::vector<std::string> Dinputs = task->getInputs();
    foreach_(std::string name, Dinputs)
      cout<<"Input: "<<name<<endl;

    std::vector<std::string> Doutputs = task->getOutputs();
    foreach_(std::string name, Doutputs)
      cout<<"Output: "<<name<<endl;

  } 
}

void IFEVisitor::CombineTasks(){
}

void IFEVisitor::CombineSegments(){
}

void IFEVisitor::OptimiseConfigurations(){
}

void IFEVisitor::GenerateSolutions(){
}

