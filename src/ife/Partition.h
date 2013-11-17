#ifndef PARTITION_H_
#define PARTITION_H_

#include "Configuration.h"

#include <vector>
#include <string>

class Partition{

  std::vector<Configuration*> partition;
  std::string name;
  double executionTime;

 public:

  Partition(std::string name){this->name = name;};
  void addConfiguration(Configuration* conf){ partition.push_back(conf);}
  void popConfiguration(){ partition.pop_back();}
  std::vector<Configuration*> getPartition(){ return partition;}
  std::string getName(){return name;}
  void setexecutionTime(double time){this->executionTime = time;}
  double getexecutionTime(){return this->executionTime;}
};

#endif // PARTITION_H_
