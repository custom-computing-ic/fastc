#include "precompiled.hxx"

#include "main.hxx"
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include <fstream>

#include "AstToDfgVisitor.hxx"
#include "AstToMaxJVisitor.hxx"
#include "KernelVisitor.hxx"
#include "PragmaVisitor.hxx"

#include <boost/filesystem.hpp>

void setupBuild() {
  using namespace boost::filesystem;
  path dir("build"), cpu("build/cpu"), engine("build/engine");
  create_directory(dir);
  create_directory(cpu);
  create_directory(engine);
}

int step = 1;

void logPass(string msg) {
  printf("[%*d] %s Pass\n", 2, step, msg.c_str());
  step++;
}

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

    //    cleanuBuild();
    setupBuild();

    Design* design = new Design();

    logPass("Kernel Extraction");
    KernelVisitor kernelVisitor(design);
    kernelVisitor.traverse(project, preorder);

    // visit kernel pragmas
    logPass("Manager Extraction");
    PragmaVisitor pragmaVisitor(design);
    pragmaVisitor.traverse(project,  preorder);

    logPass("Memory Write Kernel Generation");

    logPass("Memory Read Kernel Generation");

    logPass("Kernel Replication");

    logPass("Design Optimisation");

    logPass("Design Code Generation");
    design->writeEngineFiles("build/engine");

    logPass("Host Interface Code Generation");

    //design->generateCode(cout);

    //design->writeCodeFiles();
    log("Dataflow Implenetation Generated Succesfully in build/");

    generateDOT(*project);



    return 0;
}
