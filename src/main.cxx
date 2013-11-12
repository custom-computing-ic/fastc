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
    c->addPass(new CodeGeneration());
    c->runPasses();

    //To check: somehow the dot file is not written now? 
    DataFlowGraph *dfg = c->getDesign()->getDataFlowGraph();
    cout<<dfg<<endl;
    if (dfg != NULL)
      DotPrint::writeDotForDfg("main_flow.dot", c->getDesign()->getDataFlowGraph());

    cout << "Stencil summary:" << endl;
    
    int num = 1;
    foreach_(Stencil* s, c->getDesign()->getStencils()) {
      cout << "Stencil " << num << endl;
      cout << "\tUpdate: " << s->getDestination() << " <-- " << s->getSource() << endl;
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

      cout << "\tOffsets " << endl;
      foreach_(StencilOffset* of, s->getOffsets()) {
	map<string, int>::iterator it;
	cout << "\t\tvars: ";
	for (it = of->var_offset.begin(); it != of->var_offset.end(); it++) {
	  cout << it->first << " " << it->second << ",";
	}
	cout << " dims: ";
	for (it = of->dim_offset.begin(); it != of->dim_offset.end(); it++) {
	  cout << it->first << " " << it->second << ",";
	}
	cout << endl;
      }
      num++;
    }


    return 0;
}
