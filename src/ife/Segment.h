#ifndef SEGMENT_H_
#define SEGMENT_H_

#include "../DfeTask.hxx"

#include <vector>
#include <string>

class Segment{

  std::vector<DfeTask*> segment;
  std::string name;

 public:
  Segment(std::string name){this->name = name;};
  void addTask(DfeTask* task){ segment.push_back(task);}
  std::vector<DfeTask*> getTasks(){ return segment;}
  std::string getName(){return name;}
};

#endif
