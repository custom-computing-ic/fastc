#ifndef COMPILER_HXX
#define COMPILER_HXX

#include "passes/Pass.hxx"

class Compiler
{

  Design *design;
  list<Pass*> passes;

  int pass;

  void logPass(string msg) {
    printf("[%*d] %s Pass\n", 2, pass, msg.c_str());
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
    foreach_ (Pass* p, passes) {
      logPass(p->logPass());
      p->runPass(design);
    }
  }

};

#endif
