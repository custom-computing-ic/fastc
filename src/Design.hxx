#ifndef DESIGN_HXX_
#define DESIGN_HXX_

#include <list>
#include <ostream>
#include "Kernel.hxx"
#include "precompiled.hxx"
#include "DataFlowGraph/DataFlowGraph.hxx"

using namespace std;


/** Stores information about a design including the original ROSE
    project, its kernels and corresponding dataflow graph. This is
    used throughout the compilation process to store and transmit
    information between compiler passes.*/
class Design {

  SgProject* project;
  list<Kernel*> kernels;
  DataFlowGraph* dfg;

public:
  Design(SgProject* project) {
    this->project = project;
  }
  void generateCode(ostream& out);
  void writeCodeFiles();
  void addKernel(Kernel* k);
  void writeEngineFiles(string path);
  void writeCPUFiles();
  Kernel* getKernel(string functionName);
  list<Kernel*> getKernels() {
    return kernels;
  }

  SgProject* getProject() {
    return project;
  }
  void setDataFlowGraph(DataFlowGraph* dfg) {
    this->dfg = dfg;
  }
};

#endif /* DESIGN_HXX_ */
