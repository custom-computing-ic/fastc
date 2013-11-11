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
#include "DotDFSVisitor.hxx"
#include "DotPrint.hxx"
#include "Stencil.h"

#include "passes/Passes.hxx"
#include "highlevel/HighlevelAnalysis.hxx"

#include "ife/IdlefunctionElimination.hxx"

#include <unistd.h>



int main(int argc, char** argv) {
    SgProject* project = frontend(argc, argv);
    //  AstTests :: runAllTests(project);
    if (project == NULL) {
        cerr << "Could not run compiler frontend! Shutting down! " << endl;
        return 1;
    }

    
    
    bool stencil_compiler_flag = false;

    DBG(stencil_compiler_flag);

    generateDOT(*project);
    setupBuild();

    Compiler* c = new Compiler(project);
    if (!stencil_compiler_flag) {
      c->addPass(new KernelExtraction());
      c->addPass(new ExtractDesignConstants());
      c->addPass(new PragmaExtraction());
      c->addPass(new BuildDFG());
      c->addPass(new PrintDotDFG());

      //c->addPass(new InputOutputExtraction());
      //c->addPass(new InlineKernels());

      c->addPass(new TaskExtraction());
    
      c->addPass(new IdlefunctionElimination());
      c->addPass(new CodeGeneration()); 
    } else {

      // stencil compiler

      c->addPass(new ExtractStencil());
    }



    /*
    c->addPass(new RemoveFast());
    c->addPass(new HostCodeGeneration());
    */
    c->runPasses();

//  DfeGraph *dfe = c->getDesign()->getDfeGraph();
//  if (dfe != NULL)
//    DotPrint::writeDotForDfg("main_flow.dot", c->getDesign()->getDfeGraph());

    if (stencil_compiler_flag) {
      cout << "Stencil summary:" << endl;
    
      int num = 0;
      foreach_(Stencil* s, c->getDesign()->getStencils()) {
	cout << "Stencil " << num << endl;
	cout << "\tDimension: " << s->getDimension() << endl;
	cout << "\tInputs: ";
	foreach_(string i, s->getInputs()) {
	  cout << i << " ";
	}
	cout << endl;

	cout << "\tOutputs: ";
	foreach_(string i, s->getOutputs()) {
	  cout << i << " ";
	}
	cout << endl;

	cout << "\tLoop variables: ";
	foreach_(string i, s->getLoopVariables()) {
	  cout << i << " ";
	}
	cout << endl;

	num++;
      }
    }

    return 0;
}
