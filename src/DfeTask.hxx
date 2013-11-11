#ifndef DFETASK_HXX_
#define DFETASK_HXX_

#include <vector>
#include <list>
#include <string>
#include "DataFlowGraph/Node.hxx"
#include "Kernel.hxx"


class DfeTask : public Node {

  static int s_idCount;
  std::vector<std::string> inputs, outputs, call_args;
  Kernel* kernel;
  
public:
  double BRAMs, LUTs, FFs, DSPs;
  double bandwidth;
  list<Offset*> streams;
  int idle; 

  std::vector<Offset*> sinks;
  std::vector<Offset*> sources;

  DfeTask(string name, Kernel *k, std::vector<std::string> call_args);

  Kernel* getKernel() { return kernel; }

  std::vector<std::string> getInputs() { return inputs; }

  std::vector<std::string> getOutputs() { return outputs; }

  std::string getName() { return name; }

  virtual std::string toMaxJ() { return "NOTHING"; }

  virtual std::string classname() {return "DfeTaskNode";}

  /** Returns the name of the kernel parameter corresponding to
      'taskParam'. Example: for the kernel definition kernel_f(int* a,
      int* b) and the function call kernel_f(in, out), 'a'
      corresponds to 'in' and 'b' to 'out'. */
  string getCorrespondingKernelParam(string taskParam);

  virtual std::string toDot() {
    string params = "";
    foreach_(string p, call_args) {
      params += p + "_" + getCorrespondingKernelParam(p) + "__";
    }
    return getName() + "params__" + params;
  }

};


#endif /*  */
