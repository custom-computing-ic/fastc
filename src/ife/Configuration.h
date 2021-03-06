#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "Segment.h"
#include "../Kernel.hxx"

#include <vector>
#include <string>

class Configuration{

  std::vector<Segment*> configuration;
  std::string name;
  Kernel* mergedKernel;
  double executionTime;

 public:
  int level;
  double BRAMs, LUTs, FFs, DSPs;
  double bandwidth;
  double Ib, Il, If, Id;
  double Ab, Al, Af, Ad;
  double Abw;

  int P;

  Configuration(std::string Cname){
    BRAMs=0; LUTs=0;   FFs=0;     DSPs=0;  bandwidth=0; name = Cname;
    Ib=218;  Il=34809; If=55061;  Id=0;

    ifstream filenode("./platform.txt");
    string ab, al, af, ad, abw;
    //skip the first line
    string dummyLine;
    getline(filenode, dummyLine);
    //read in the second line as the platform file
    filenode>>ab>>al>>af>>ad>>abw;
    //cout<<ab<<al<<af<<ad<<abw<<endl;
    istringstream(ab)>>Ab;
    istringstream(al)>>Al;
    istringstream(af)>>Af;
    istringstream(ad)>>Ad;
    istringstream(abw)>>Abw;
    //cout<<Ab<<Al<<Af<<Ad<<Abw<<endl;

    //Ab=1064; Al=297600;Af=595200; Ad=2016; Abw= 28800;//Virtex-6
    //Ab=2567; Al=695000;Af=1050000;Ad=1963; Abw= 65000;//Stratix-V
    P =123456;
  };
  void addSegment(Segment* segment){ configuration.push_back(segment);}
  std::vector<Segment*> getConfiguration(){ return configuration;}
  std::string getName(){return name;}
  void setName(std::string Cname){name = Cname;}
  double getReconfigurationTime(){return 0.8;}
  void setMergedKernel(Kernel* mergedKernel) { 
    this->mergedKernel = mergedKernel; 
  }
  void setexecutionTime(double time){this->executionTime = time;}
  double getexecutionTime(){return this->executionTime;}
};

#endif // CONFIGURATION_H_
