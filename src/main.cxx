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

#include "passes/Passes.hxx"
#include "highlevel/HighlevelAnalysis.hxx"

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
    c->addPass(new ExtractDesignConstants());
    c->addPass(new PragmaExtraction());
    c->addPass(new BuildDFG());
    c->addPass(new PrintDotDFG());
    c->addPass(new HighlevelAnalysis());

    //    c->addPass(new InputOutputExtraction());
    //    c->addPass(new InlineKernels());
    c->addPass(new CodeGeneration());
    //    c->addPass(new RemoveFast());
    //    c->addPass(new TaskExtraction());
    //    c->addPass(new HostCodeGeneration());
    c->runPasses();
    return 0;
}
