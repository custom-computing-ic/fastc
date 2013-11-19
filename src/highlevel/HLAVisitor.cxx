#include "HLAVisitor.hxx"
#include <sstream>
#include <math.h>       

HLAVisitor::HLAVisitor(Kernel *k){
  this->kernel = k;
  this->dfg = k->getDataFlowGraph();
  //operating frequency is fixed for now
  this->frequency = 100;
  this->offchipdata = 0;

  this->BRAMs =0;
  this->LUTs=0;
  this->FFs =0;
  this->DSPs=0;
  
  this->internaldelay =0;
  this->inputdelay =0;
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
  //TODO: offsets are variable, in that case, consider parse
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
  {
    //currently we clean the previous results for each DfeTask analysis
    //as the offset nodes for DfeTask are shared
    (*it)->internaldelay = 0;
    D(cout<<"stream "<<(*it)->getName()<<endl;)
    D(cout<<"offset:"<<endl;)
    precision = (*it)->precision[0] + (*it)->precision[1]; //mantisa + significant
    
    if(((*it)->getStencilOffsets()).size()!=0)//if there is a stencil
    {
      //TODO: check why the iterator doesn't work 
      foreach_(StencilOffset* soff, ((*it)->getStencilOffsets()))
      {
        vector<string> loopVars = soff->stencil->getLoopVariables();
        Pair* pair = new Pair();
        foreach_(string key, loopVars)
        {
          vector<int>::iterator front; 
          front = pair->offsets.begin();
          pair->offsets.insert(front, soff->var_offset[key]);
          //some hacking here: assume " " stands for 0
          if(soff->var_dim[key] == "") soff->var_dim[key] ="0";
          int offsetbuf;
          istringstream (soff->var_dim[key]) >> offsetbuf;
          front = pair->dimensions.begin();
          pair->dimensions.insert(front,offsetbuf);
        }

        //invert if to fastest dimension -> lowest dimension (begin -> end)
        //extract the dimension size for each dimension
        //some hacking here: assume the thrid dimension size is the same as second
        for(vector<int>::iterator dims = pair->dimensions.begin(); dims != pair->dimensions.end(); dims++)
        {
          if(dims == pair->dimensions.begin())
            *dims = *(dims+1);
          else if(dims == (pair->dimensions.begin() + pair->dimensions.size() -1 ))//slowest dimension
            *dims = 512;//*dims = *(dims-1);
          else
            *dims =(*(dims+1)) / (*dims);
        }
        (*it)->pairs.push_back(pair); 

#if DEBUG
        foreach_(string key, loopVars)
          cout<<key<<" ";
        for(vector<int>::iterator debug = pair->dimensions.begin(); debug != pair->dimensions.end(); debug++)
          cout<<*debug<<" ";
        for(vector<int>::iterator debug = pair->offsets.begin();    debug != pair->offsets.end(); debug++)
          cout<<*debug<<" ";
        cout<<endl;
#endif
      }


      //extract the maximum and minimum offset for each dimension
      //for each dimension, we need to go through all the dimensions
      //as offset value are for the same data (stream), the dimension size is the same
      int size = ((*it)->pairs.front())->dimensions.size();
      (*it)->max.resize(size); 
      (*it)->min.resize(size); 
      D(cout<<"size "<<size<<endl;)

      foreach_(Pair* pair,(*it)->pairs)
      {
        vector<int>::iterator jt_max = (*it)->max.begin(); 
        vector<int>::iterator jt_min = (*it)->min.begin();
        for(vector<int>::iterator jt = pair->offsets.begin(); jt != pair->offsets.end(); jt++)
        {
          *jt_max = (*jt_max) > (*jt) ? (*jt_max) : (*jt);
          *jt_min = (*jt_min) < (*jt) ? (*jt_min) : (*jt);
          jt_max++;
          jt_min++;
        }
      }

#if DEBUG
      foreach_(int value, (*it)->max)
        cout<<"max "<<value<<" ";
      cout<<endl;
      foreach_(int value, (*it)->min)
        cout<<"min "<<value<<" ";
      cout<<endl;
#endif

      //TODO: data blocking
      for(vector<int>::iterator jt_max = (*it)->max.begin(); jt_max != (*it)->max.end(); jt_max++)
      {
        vector<int>::iterator jt_min = (*it)->min.begin();

        int gap=1;//distance for 1 neighbouring data in this dimension
        int dimGap = jt_max - (*it)->max.begin();//the 1 account for that first dimenion gap is 1
        for(vector<int>::iterator jt_dim =((*it)->pairs.front())->dimensions.begin(); 
                                  jt_dim!=((*it)->pairs.front())->dimensions.begin()+dimGap; jt_dim++)
          gap = gap * (*jt_dim);

        (*it)->internaldelay += ((*jt_max) - (*jt_min)) * gap;
        jt_min++;
      }
      D(cout<<"internal delay: "<<(*it)->internaldelay<<endl;)
        
      (*it)->BRAMs = (double) (((*it)->internaldelay) * precision) / (36.0 * 1024.0);
      double width = (double)((int)((*it)->OnchipMemory.size()) * precision) / 36.0;
      if((*it)->BRAMs ==0) width = 0;

      (*it)->BRAMs = (*it)->BRAMs > width ? (*it)->BRAMs : width;
      (*it)->BRAMs =(double) ceil((*it)->BRAMs);
    }
    else//for kernel with only 1 dimension 
    {
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

        int Max, Min;
        Max=0;
        Min=0;
        for(list<int>::iterator jt = (*it)->OnchipMemory.begin(); jt!=(*it)->OnchipMemory.end(); jt++)
        {
          Max = Max > *jt ? Max : *jt;
          Min = Min > *jt ? *jt : Min;
        } 
        (*it)->internaldelay = Max - Min;
        (*it)->BRAMs = (double) ((Max - Min) * precision) / (36.0 * 1024.0);

        double width = (double)((int)((*it)->OnchipMemory.size()) * precision) / 36.0;
        if((*it)->BRAMs ==0) width = 0;

        (*it)->BRAMs = (*it)->BRAMs > width ? (*it)->BRAMs : width;
        (*it)->BRAMs =(double) ceil((*it)->BRAMs);
      }
    }
  }

  //aggregrate BRAMs
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
    BRAMs += (*it)->BRAMs;
  cout<<"     memory resource consumption: "<< BRAMs<<" BRAMs"<<endl; 
  
  //aggregrate idle cycles
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
    this->internaldelay = this->internaldelay > (*it)->internaldelay ? this->internaldelay : (*it)->internaldelay;
  cout<<"     kernel internal delay: "<< this->internaldelay<<" cycles"<<endl;

  //calculate data size 
  this->ds = 1; 
  foreach_(int size, (((dfg->streams.front())->pairs).front())->dimensions)
    this->ds = this->ds * size;
  cout<<"     data size: "<<this->ds<<endl;
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
  for(list<Offset*>::iterator it = dfg->streams.begin(); it!=dfg->streams.end(); it++)
  {
    D(cout<<"stream "<<(*it)->getName()<<endl;)
    boost::cmatch group;
    boost::regex* r = new boost::regex("[a-zA-Z]*\\(([0-9]*),\\s*([0-9]*)\\)");
    if (boost::regex_match((kernel->ioTypeMap)[(*it)->getName()].c_str(), group, *r))
    {
      int significant;
      istringstream(group[1]) >> significant;
      int mantissa;
      istringstream(group[2]) >> mantissa;
      precision = significant + mantissa;
    }
    else 
      precision = (*it)->precision[0] + (*it)->precision[1];//default value

    cout<<(kernel->ioTypeMap)[(*it)->getName()]<<endl;
    cout<<"precision: "<<precision<<endl;
   
    //bandwidth calculation is now taken care at the configuration level
    //bandwidth for each stream are recorded separately
    //TODO: use gap(*it) to calculate the data required from onchipmemory, evey cycle 
    (*it)->bandwidth = (double) precision * frequency / 8;
#if DEBUG    
    cout<<"bandwidth: "<<(*it)->bandwidth<<" MB/s"<<endl;
#endif

    //offchipdata += (double) precision * gap(*it);  
    //bandwidth = offchipdata * frequency / 8;
  } 
}

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
          Fs = 100+Ls;
          Ds = 0; 
        } 
        else if (node->getName()=="*")
        {
          Ls = 673;
          Fs = 143+Ls;
          Ds = 0;
        }
        break;
      case 1:
        if(node->getName() == "+" || node->getName() == "-")
        {
          Ls = 209;
          Fs = 100+Ls;
          Ds = 2; 
        } 
        else if (node->getName()=="*")
        {
          Ls = 100;
          Fs = 51+Ls;
          Ds = 2;
        }
        break;
      case 2:
        if(node->getName() == "+" || node->getName() == "-")
        {
          Ls = 209;
          Fs = 100+Ls;
          Ds = 2; 
        } 
        else if (node->getName()=="*")
        {
          Ls = 114;
          Fs = 50+Ls;
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
    D(cout<<"Ls: "<<Ls<<" Fs: "<<Fs<<" Ds: "<<Ds<<endl;)
  }
}


void HLAVisitor::ArithmeticAnalysis(){


  for(list<Node*>::iterator it = dfg->arithmetics.begin(); it!=dfg->arithmetics.end(); it++)
  {

    D(cout<<"arith node "<<(*it)->getName()<<endl;)
    D(cout<<"input: ";)

    int width[2] ={0,0};
    for(list<Node*>::iterator jt = (*it)->inputs.begin(); jt!=(*it)->inputs.end(); jt++)
    {
      D(cout<<" "<<(*jt)->getName();)
      width[0] = width[0] > (*jt)->precision[0] ? width[0] : (*jt)->precision[0];
      width[1] = width[1] > (*jt)->precision[1] ? width[1] : (*jt)->precision[1];
    }
    ArithmeticResource((*it), width);
    D(cout<<endl;)
    LUTs += Ls;
    FFs  += Fs;
    DSPs += Ds;
  }
  cout<<"     LUTs: "<<LUTs<<" FFs: "<<FFs<<" DSPs: "<<DSPs<<endl;
}
