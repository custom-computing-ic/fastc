#include "HLAVisitor.hxx"
#include <sstream>
#include <math.h>       

HLAVisitor::HLAVisitor(Kernel *k){
  this->kernel = k;
  this->dfg = k->getDataFlowGraph();
}

bool HLAVisitor::findoffset( int offset){
  list<int>::iterator it;
  for (it = OnchipMemory.begin(); it != OnchipMemory.end(); it++) {
    if ((*it) == offset)
      return true;
  }
  return false;
}

void HLAVisitor::MemoryAnalysis(){
  
  
  //calculating the distance between offsets for each strea
  //TODO: multi-dimension offsets
  //TODO: offsets are variable, in that case, consider parse
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
  {
    cout<<"stream "<<(*it)->getName()<<endl;
    cout<<"offset:";
    int precision;
    precision = (*it)->precision[0] + (*it)->precision[1];

    for(list<string>::iterator jt = (*it)->offsets.begin(); jt!=(*it)->offsets.end(); jt++)
    {
      cout<<" "<< *jt;
      int offset;
      istringstream ( *jt ) >> offset;
      if(!findoffset(offset))
        OnchipMemory.push_back(offset); 
    }
    cout<<endl;

    int max, min;
    max=0;
    min=0;
    
    //TODO: current data access pattern is at the same dimension
    for(list<int>::iterator jt = OnchipMemory.begin(); jt!=OnchipMemory.end(); jt++)
    {
      max = max > *jt ? max : *jt;
      min = min > *jt ? *jt : min;
    } 
    double Bs = (double) ((max - min) * precision) / (36.0 * 1024.0);
    
    double width = (double)((int)(OnchipMemory.size()) * precision) / 36.0;

    Bs = Bs > width ? Bs : width;
    int BRAMs = ceil(Bs);
    cout<<"memory resource consumption: "<< BRAMs<<" BRAMs"<<endl; 
    OnchipMemory.clear(); 
  }
}

