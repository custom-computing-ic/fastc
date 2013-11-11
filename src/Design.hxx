#ifndef DESIGN_HXX_
#define DESIGN_HXX_

#include <list>
#include <ostream>
#include "Kernel.hxx"
#include "precompiled.hxx"
#include "DfeGraph.hxx"
#include "DfeTask.hxx"
#include "Stencil.h"
#include "utils.hxx"

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
  list<Kernel*> kernels;
  DfeGraph* dfe;
  map<string, set<string> > inputs_to_fname, outputs_to_fname;
  vector<Stencil*> stencils;

public:
  Design(SgProject* project) {
    this->project = project;
    this->dfe = new DfeGraph();
  }

  void generateCode(ostream& out);
  void writeCodeFiles();
  void addKernel(Kernel* k);
  void writeEngineFiles(string path);
  void writeCPUFiles();

  /** Adds a new DFE task to this design and updates the dependecies
      between tasks. */
  /** XXX This method is not general enough as it does not use
      dataflow analysis to verify the dependencies but just assumes
      that tasks a task with inputs (i1, i2 .. ) depends on all tasks
      with any of the outputs (i1, i2 ...). This is, in general, not
      the if we consider control flow statements etc.*/
  void addDfeTask(DfeTask* task);

  Kernel* getKernel(string functionName);

  /** Given a function call statement, returns the kernel that is
      invoked by that function call.*/
  Kernel* getKernelMatchingFunctionCall(SgFunctionCallExp *);

  list<Kernel*> getKernels() {
    return kernels;
  }

  SgProject* getProject() {
    return project;
  }

  DfeGraph* getDfeGraph() { return dfe; }


  void addStencil(Stencil* s) { stencils.push_back(s); }
  vector<Stencil*> getStencils() { return stencils; }
};

#endif /* DESIGN_HXX_ */
