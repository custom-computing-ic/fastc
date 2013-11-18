#ifndef COMPILER_HXX
#define COMPILER_HXX

#include "passes/Pass.hxx"

class Compiler
{

  Design *design;
  list<Pass*> passes;

  int pass;


  void log(string msg) {
    printf("%s\n", msg.c_str());
  }

  void logPass(string msg) {
    printf("\033[1;33m[%*d] %s Pass\n\033[0m", 2, pass, msg.c_str());
    pass++;
  }

public:

  Compiler(SgProject* project) {
    pass = 0;
    design = new Design(project);
  }

  void addPass(Pass* p) {
    passes.push_back(p);
  }

  void runPasses() {
    log("Starting Dataflow Compilation");
    foreach_ (Pass* p, passes) {
      logPass(p->logPass());
      p->runPass(design);
    }
    log("Dataflow Implementation Generated Succesfully in build/");
  }

  Design* getDesign() { return design; }

  Kernel* getKernel(string functionName) { return design->getKernel(functionName); }

};

#endif
