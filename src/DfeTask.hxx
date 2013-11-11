#ifndef DFETASK_HXX_
#define DFETASK_HXX_ 

#include <vector>
#include <list>
#include <string>
#include "DataFlowGraph/Node.hxx"
#include "Kernel.hxx"

class DfeTask : public Node {

  static int s_idCount;
  std::vector<std::string> inputs, outputs;
  Kernel* kernel;
  
public:
  double BRAMs, LUTs, FFs, DSPs;
  double bandwidth;
  list<Offset*> streams; 

  std::vector<Offset*> sinks;
  std::vector<Offset*> sources;

  DfeTask(string name, Kernel *k, std::vector<std::string> call_args);

  Kernel* getKernel() { return kernel; }

  std::vector<std::string> getInputs() { return inputs; }

  std::vector<std::string> getOutputs() { return outputs; }

  std::string getName() { return name; }

  virtual std::string toMaxJ() { return "NOTHING"; }

  virtual std::string classname() {return "DfeTaskNode";}

};


#endif /*  */
