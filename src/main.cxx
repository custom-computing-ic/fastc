#include "precompiled.hxx"

#include "main.hxx"
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include <fstream>

#include "AstToDfgVisitor.hxx"
#include "AstToMaxJVisitor.hxx"
#include "PragmaVisitor.hxx"
#include "Compiler.hxx"
#include "utils.hxx"

#include "passes/ManagerExtraction.hxx"
#include "passes/RemoveFast.hxx"
#include "passes/KernelExtraction.hxx"
#include "passes/CodeGeneration.hxx"
#include "passes/HostCodeGeneration.hxx"
#include "passes/TaskExtraction.hxx"
#include "passes/PragmaExtraction.hxx"
#include "passes/ExtractDesignConstants.hxx"
#include "passes/InlineKernels.hxx"
#include "passes/InputOutputExtraction.hxx"

int main(int argc, char** argv) {

  SgProject* project = load_rose_project(argc, argv);
  setupBuild();

  Compiler* c = new Compiler(project);
  c->addPass(new KernelExtraction());
  c->addPass(new ExtractDesignConstants());
  c->addPass(new PragmaExtraction());
  //    c->addPass(new InputOutputExtraction());
  c->addPass(new InlineKernels());
  c->addPass(new CodeGeneration());
  c->addPass(new RemoveFast());
  c->addPass(new TaskExtraction());
  c->addPass(new HostCodeGeneration());
  c->runPasses();
  //    generateDOT(*project);
  return 0;
}
