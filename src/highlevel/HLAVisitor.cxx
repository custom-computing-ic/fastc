#include "HLAVisitor.hxx"

HLAVisitor::HLAVisitor(Kernel *k){
  this->kernel = k;
  this->dfg = k->getDataFlowGraph();
}


void HLAVisitor::MemoryAnalysis(){
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
  {
    cout<<"buffer "<<(*it)->getName()<<endl;
    for(list<string>::iterator jt = (*it)->offsets.begin(); jt!=(*it)->offsets.end(); jt++)
      cout<<" "<< *jt;
    cout<<endl;
  }
}

