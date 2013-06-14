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

#include "passes/ManagerExtraction.hxx"
#include "passes/RemoveFast.hxx"
#include "passes/KernelExtraction.hxx"
#include "passes/CodeGeneration.hxx"
#include "passes/HostCodeGeneration.hxx"
#include "passes/TaskExtraction.hxx"

#include <boost/filesystem.hpp>

void setupBuild() {
  using namespace boost::filesystem;
  path dir("build"), cpu("build/cpu"), engine("build/engine");
  create_directory(dir);
  create_directory(cpu);
  create_directory(engine);
}

int step = 1;

void log(string msg) {
  printf("%s\n", msg.c_str());
}

int main(int argc, char** argv) {

    SgProject* project = frontend(argc, argv);
    //  AstTests :: runAllTests(project);
    if (project == NULL) {
        cerr << "Could not run compiler frontend! Shutting down! " << endl;
        return 1;
    }

    setupBuild();

    Compiler* c = new Compiler(project);

    c->addPass(new KernelExtraction());
    c->addPass(new CodeGeneration());
    //    c->addPass(new RemoveFast());
    c->addPass(new TaskExtraction());

    //    c->addPass(new Prag)
    //    PragmaVisitor pragmaVisitor(design);
    //    pragmaVisitor.traverse(project,  preorder);

    //logPass("Manager Extraction");
    //    ManagerExtraction mep(design);
    //    mep.traverse(project, preorder);

    //logPass("Memory Write Kernel Generation");

    //logPass("Memory Read Kernel Generation");

    //logPass("Kernel Replication");

    //logPass("Design Optimisation");

    //logPass("Design Code Generation");


    //logPass("Host Interface Code Generation");
    c->addPass(new HostCodeGeneration());

    //design->generateCode(cout);

    //design->writeCodeFiles();
    log("Dataflow Implenetation Generated Succesfully in build/");
    c->runPasses();

    generateDOT(*project);

    return 0;
}
