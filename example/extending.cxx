#include "../Compiler.hxx"

// example of adding a custom property to each DFG node


// example of creating a transformation pass over the DFG


int main() {

  Compiler *c = new Compiler(project);
  c->addPass(new BuildDataFlowGraph());
  c->addPass(new PrintDataFlowGraph());
  c->runPasses();



  return 0;
}
