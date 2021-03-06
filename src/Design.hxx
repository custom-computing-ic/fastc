#ifndef DESIGN_HXX_
#define DESIGN_HXX_


#include "Kernel.hxx"
#include "precompiled.hxx"
#include "DataFlowGraph/DataFlowGraph.hxx"
#include "DfeTask.hxx"
#include "Stencil.h"
#include "utils.hxx"
#include "ife/Partition.h"

#include <list>
#include <ostream>
#include <map>
#include <set>
#include <string>

using namespace std;

/** Stores information about a design including the original ROSE
    project, its kernels and corresponding dataflow graph. This is
    used throughout the compilation process to store and transmit
    information between compiler passes.*/
class Design {

  SgProject* project;
  std::list<Kernel*> kernels;
  DataFlowGraph* dfg;
  std::map<std::string, set<std::string> > inputs_to_fname, outputs_to_fname;
  std::vector<Stencil*> stencils;
  std::vector<Partition*> partitions;

public:
  Design(SgProject* project) {
    this->project = project;
    this->dfg = new DataFlowGraph();
  }

  void generateCode(ostream& out);
  void writeCodeFiles();
  void addKernel(Kernel* k);
  void writeEngineFiles(std::string path);
  void writeCPUFiles();

  /** Adds a new DFE task to this design and updates the dependecies
      between tasks. */
  /** XXX This method is not general enough as it does not use
      dataflow analysis to verify the dependencies but just assumes
      that tasks a task with inputs (i1, i2 .. ) depends on all tasks
      with any of the outputs (i1, i2 ...). This is, in general, not
      the if we consider control flow statements etc.*/
  void addDfeTask(DfeTask* task);

  Kernel* getKernel(std::string functionName);

  /** Given a function call statement, returns the kernel that is
      invoked by that function call.*/
  Kernel* getKernelMatchingFunctionCall(SgFunctionCallExp *);

  std::list<Kernel*> getKernels() {
    return kernels;
  }

  SgProject* getProject() {
    return project;
  }

  DataFlowGraph* getDataFlowGraph() { return dfg; }
  void addStencil(SgFunctionDeclaration* f, Stencil* s);
  std::vector<Stencil*> getStencils() { return stencils; }
  void setPartitions(std::vector<Partition*> partitions) {
    this->partitions = partitions;
  }
  std::vector<Partition* > getPartitions() { return partitions; }
};

#endif /* DESIGN_HXX_ */
