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

#include "ife/Configuration.h"
#include "ife/Partition.h"
#include "ife/IdlefunctionElimination.hxx"

#include <unistd.h>

#define TD "<TD>"
#define ETD "</TD>"
#define CTD "</TD><TD>"
#define TABLE "<TABLE>"
#define ETABLE "</TABLE>"
#define TR "<TR>"
#define ETR "</TR>"


string colors[] = {"red", "green", "blue"};


void write_dot_analysis(Design *design) {

  ofstream f("nice_graph.dot");

  DataFlowGraph* dfg = design->getDataFlowGraph();

  int n_cluster = 0;
  map<string, string> colors_map;
  foreach_(Partition* p, design->getPartitions()) {
    f << "digraph G {" << endl;
    f << "subgraph cluster" << n_cluster;
    n_cluster++;
    f << " { " << endl;
    f << " node[style=filled, color=white];" << endl;
    f << "resource [shape=plaintext, label = < " << endl;
    f << TABLE << endl;
    f << TR << TD << CTD << "BRAMs" << CTD << "LUTs" << CTD << "FFs" << CTD << "DSPs";
    f << CTD << "Par" << CTD << "Exec. Time" << ETD << ETR << endl;
    colors_map.clear();
    int i = 0;
    foreach_ (Configuration *c, p->getPartition()) {
      string color = colors[i++];
      f << "<TR>" << endl;
      f << TD << "<FONT COLOR=\"" + color + "\"> Configuration ";
      f << c->getName() << "</FONT>" << ETD;
      f << TD << c->BRAMs << ETD;
      f << TD << c->LUTs << ETD;
      f << TD << c->FFs << ETD;
      f << TD << c->DSPs << ETD;
      f << TD << c->P << ETD;
      f << TD << c->getexecutionTime() << ETD;
      f << "</TR>" << endl;
      foreach_(Segment* s, c->getConfiguration()) {
        foreach_ (DfeTask* d, s->getDfeTasks()) {
          colors_map[d->getName()] = color;
        }
      }
    }
    f << TR << TD << "TOTAL" << ETD << endl;
    f << TD << CTD << CTD << CTD << CTD << CTD << endl;
    f << p->getexecutionTime() << ETD;
    f << ETR;

    f << "</TABLE>" << endl;
    f << ">];" << endl;
    f << "style = filled;\n color=lightgrey;\n fontsize=20; " << endl;
    f << "label = \"Partition " << p->getName() << "\";" << endl;
    f << "}" << endl;

    foreach_(Node* n, dfg->getNodes()) {
      DfeTask* task = dynamic_cast<DfeTask*>(n);
      if (task == NULL)
        continue;
      string taskName = task->getName();
      f << taskName << "[label = \"";
      f << taskName;
      f << "\", fontcolor=\"" + colors_map[taskName];
      f << "\"];\n";
    }
    DotPrint::writeDotEdges(f, dfg);
    f << "}" << endl;
  }

  f.close();
}

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
  c->addPass(new MergeKernels());

  c->addPass(new CodeGeneration());
  c->runPasses();

  //To check: somehow the dot file is not written now?
  DataFlowGraph *dfg = c->getDesign()->getDataFlowGraph();
  ofstream ff("main_flow.dot");
  if (dfg != NULL)
    DotPrint::writeDotForDfg(ff, dfg);
  ff.close();

  write_dot_analysis(c->getDesign());

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
