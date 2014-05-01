#ifndef DFEGRAPH_HXX_
#define DFEGRAPH_HXX_

#include <list>
#include "Kernel.hxx"
#include "DfeTask.hxx"

class DfeGraph {
  public: 
  DfeTask *source, *sink;
  std::list<DfeTask*>  tasks;
  std::list<DfeTask*>  sources;
  
  DfeGraph();
//void      addInputTask(DfeTask*);
//void      addOutputTask(DfeTask*);
  DfeTask*  findTask(string name);
  DfeTask*  addSource(DfeTask*);
  DfeTask*  addTask(DfeTask *n);

  std::list<DfeTask *> getSources();
  std::list<DfeTask *> getTasks();
};
#endif /* DFEGRAPH_HXX_ */

