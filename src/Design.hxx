#ifndef DESIGN_HXX_
#define DESIGN_HXX_

#include <list>
#include <ostream>
#include "Kernel.hxx"
#include "precompiled.hxx"

using namespace std;

class Design {

  SgProject* project;
  list<Kernel*> kernels;

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
};

#endif /* DESIGN_HXX_ */
