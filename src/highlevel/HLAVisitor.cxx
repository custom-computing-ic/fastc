#include "HLAVisitor.hxx"
#include <sstream>
#include <math.h>       

HLAVisitor::HLAVisitor(Kernel *k){
  this->kernel = k;
  this->dfg = k->getDataFlowGraph();
  //operating frequency is fixed for now
  this->frequency = 100;
  this->offchipdata = 0;
}

bool HLAVisitor::findoffset( int offset, Offset* node){
  list<int>::iterator it;
  for (it = node->OnchipMemory.begin(); it != node->OnchipMemory.end(); it++) {
    if ((*it) == offset)
      return true;
  }
  return false;
}
void HLAVisitor::OnchipMemoryAnalysis(){
  
  
  //calculating the distance between offsets for each strea
  //TODO: multi-dimension offsets
  //TODO: offsets are variable, in that case, consider parse
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
  {
    cout<<"stream "<<(*it)->getName()<<endl;
    cout<<"offset:";
    precision = (*it)->precision[0] + (*it)->precision[1];

    for(list<string>::iterator jt = (*it)->offsets.begin(); jt!=(*it)->offsets.end(); jt++)
    {
      cout<<" "<< *jt;
      int offset;
      istringstream ( *jt ) >> offset;
      if(!findoffset(offset, *it))
        (*it)->OnchipMemory.push_back(offset); 
    }
    cout<<endl;

    int max, min;
    max=0;
    min=0;
    
    //TODO: current data access pattern is at the same dimension
    for(list<int>::iterator jt = (*it)->OnchipMemory.begin(); jt!=(*it)->OnchipMemory.end(); jt++)
    {
      max = max > *jt ? max : *jt;
      min = min > *jt ? *jt : min;
    } 
    Bs = (double) ((max - min) * precision) / (36.0 * 1024.0);
    
    double width = (double)((int)((*it)->OnchipMemory.size()) * precision) / 36.0;
    if(Bs ==0) width = 0;

    Bs = Bs > width ? Bs : width;
    int BRAMs = ceil(Bs);
    cout<<"memory resource consumption: "<< BRAMs<<" BRAMs"<<endl; 
    //OnchipMemory.clear(); 
  }
}

double HLAVisitor::gap(Offset* node)
{

  //TODO: when multiple dimension are involved, 
  //overlap and different between OnchipMemory and nextStep would be 
  //more complicated 
  //TODO: partial storage (block tiling)
  list<int> nextStep;
  for(list<int>::iterator it = node->OnchipMemory.begin(); it!=node->OnchipMemory.end(); it++)
    nextStep.push_back(*it+1);

  double difference; 
  for(list<int>::iterator it = nextStep.begin(); it!=nextStep.end(); it++)
  {    
    int tag =1;
    for(list<int>::iterator jt = node->OnchipMemory.begin(); jt!=node->OnchipMemory.end(); jt++)
    if(*it <= * jt) tag =0;

    if(tag == 1) difference +=1;
  } 
  return difference;
}

void HLAVisitor::OffchipCommunicationAnalysis(){

  //TODO: when there are multiple kernels (fucntions), communication
  //patterns can overlap, need to be supproted
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
  {
    cout<<"stream "<<(*it)->getName()<<endl;
    precision = (*it)->precision[0] + (*it)->precision[1];
    offchipdata += (double) precision * gap(*it);  
    bandwidth = offchipdata * frequency / 8;

    cout<<"bandwidth: "<<bandwidth<<" MB/s"<<endl;
  } 
}

//void HLAVisitor::TransFactor(){
  
//}

void HLAVisitor::ArithmeticResource(Node* node, int* width){
  Ls=0;
  Fs=0;
  Ds=0;
  if(node->floating)
  {
    switch(node->transformation) {
      case 0:
        if(node->getName() == "+" || node->getName() == "-")
        {
          Ls = 209;
          Fs = 100;
          Ds = 0; 
        } 
        else if (node->getName()=="*")
        {
          Ls = 673;
          Fs = 143;
          Ds = 0;
        }
        break;
      case 1:
        if(node->getName() == "+" || node->getName() == "-")
        {
          Ls = 209;
          Fs = 100;
          Ds = 2; 
        } 
        else if (node->getName()=="*")
        {
          Ls = 100;
          Fs = 51;
          Ds = 2;
        }
        break;
      case 2:
        if(node->getName() == "+" || node->getName() == "-")
        {
          Ls = 209;
          Fs = 100;
          Ds = 2; 
        } 
        else if (node->getName()=="*")
        {
          Ls = 114;
          Fs = 50;
          Ds = 3;
        }
      default: cout<<"invalid transformation ratio"<<endl;
    }
  }
  else
  {
    switch(node->transformation) {
      case 0:
        if(node->getName() == "+" || node->getName() == "-")
        {
          Ls = width[0] + width[1];
          Fs = width[0] + width[1];
        } 
        else if (node->getName()=="*")
        {
          Ls = (width[0] + width[1]) * (width[0] + width[1]);
          Fs =  width[0] + width[1];
        }
        break; 
      case 1:
        if(node->getName() == "+" || node->getName() == "-")
        {
          Ls = width[0] + width[1];
          Fs = width[0] + width[1];
        }
        else if (node->getName()=="*")
        {
          int widths = width[0] + width[1];
          if(widths<=18)
            Ds=1;
          else if(widths<=25)
            Ds=2;
          else if(widths<=35)
            Ds=4;
          else if(widths<=42)
            Ds=5;
          else if(widths<=52)
            Ds=9;
          else if(widths<=59)
            Ds=10;
          else if(widths<=64)
            Ds=16;
        }
        break;
      case 2:
        if(node->getName() == "+" || node->getName() == "-")
          Ds += (int) ((width[0]+width[1]) / 18) + 1;
        else if (node->getName()=="*")
        {
          int widths = width[0] + width[1];
          if(widths<=18)
            Ds=1;
          else if(widths<=25)
            Ds=2;
          else if(widths<=35)
            Ds=4;
          else if(widths<=42)
            Ds=5;
          else if(widths<=52)
            Ds=9;
          else if(widths<=59)
            Ds=10;
          else if(widths<=64)
            Ds=16;
        }
        break;
      default: cout<<"invalid transformation ratio"<<endl;
    }
    cout<<"Ls: "<<Ls<<" Fs: "<<Fs<<" Ds: "<<Ds<<endl;
  }
}


void HLAVisitor::ArithmeticAnalysis(){

  LUTs=0;
  FFs =0;
  DSPs=0;

  for(list<Node*>::iterator it = dfg->arithmetics.begin(); it!=dfg->arithmetics.end(); it++)
  {
    cout<<"arith node "<<(*it)->getName()<<endl;
    cout<<"input: ";
    
    int width[2] ={0,0};
    for(list<Node*>::iterator jt = (*it)->inputs.begin(); jt!=(*it)->inputs.end(); jt++)
    {
      cout<<" "<<(*jt)->getName();
      width[0] = width[0] > (*jt)->precision[0] ? width[0] : (*jt)->precision[0];
      width[1] = width[1] > (*jt)->precision[1] ? width[1] : (*jt)->precision[1];
    }
    ArithmeticResource((*it), width);
    cout<<endl;
    LUTs += Ls;
    FFs  += Fs;
    DSPs += Ds;
  }
  cout<<"LUTs: "<<LUTs<<" FFs: "<<FFs<<" DSPs: "<<DSPs<<endl;
}
