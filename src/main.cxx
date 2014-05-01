#include "precompiled.hxx"

#include "main.hxx"
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include <fstream>

#include "AstToDfgVisitor.hxx"
#include "AstToMaxJVisitor.hxx"
#include "Compiler.hxx"
#include "DotDFSVisitor.hxx"
#include "DotPrint.hxx"
#include "Stencil.h"

#include "passes/Passes.hxx"
#include "highlevel/HighlevelAnalysis.hxx"

#include "ife/Configuration.h"
#include "ife/Partition.h"
#include "ife/IdlefunctionElimination.hxx"

#include <unistd.h>

int main(int argc, char** argv) {
  SgProject* project = frontend(argc, argv);
  //  AstTests :: runAllTests(project);
  if (project == NULL) {
    cerr << "Could not run compiler frontend! Shutting down! " << endl;
    return 1;
  }

  generateDOT(*project);
  setupBuild();

  Compiler* c = new Compiler(project);
  c->addPass(new KernelExtraction());
  c->addPass(new ExtractStencil());
  c->addPass(new ExtractDesignConstants());
  c->addPass(new PragmaExtraction());
  c->addPass(new BuildDFG());
  c->addPass(new PrintDotDFG());
  c->addPass(new TaskExtraction());
  c->addPass(new IdlefunctionElimination());
  //c->addPass(new MergeKernels());
  c->addPass(new CodeGeneration());
  c->addPass(new GenerateAnalysisOverview());
  c->runPasses();

  //To check: somehow the dot file is not written now?
  DataFlowGraph *dfg = c->getDesign()->getDataFlowGraph();
  ofstream ff("main_flow.dot");
  if (dfg != NULL)
    DotPrint::writeDotForDfg(ff, dfg);
  ff.close();

#if DEBUG
  int num = 1;
  foreach_(Stencil* s, c->getDesign()->getStencils()) {
    cout << "Stencil " << num << endl;
    s->print();
    cout << endl;
    num++;
  }
#endif

  return 0;
}
