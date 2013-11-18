#include "GenerateAnalysisOverview.h"
#include "../DotPrint.hxx"

using namespace std;

string GenerateAnalysisOverview::analysis_overview = "analysis_overview.dot";

std::string GenerateAnalysisOverview::colors[] = {"red", "green", "blue"};


void GenerateAnalysisOverview::runPass(Design* design) {
  using namespace std;

  ofstream f(this->analysis_overview.c_str());

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

  using namespace boost::filesystem;
  path file_path(analysis_overview);
  cout << "    Generated analysis overview in " << file_path.string() << endl;
}
