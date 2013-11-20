#include "GenerateAnalysisOverview.h"
#include "../DotPrint.hxx"

#include <sstream>

using namespace std;

string GenerateAnalysisOverview::analysis_overview = "analysis_overview.dot";

std::string GenerateAnalysisOverview::colors[] = {"red", "green", "blue"};


void GenerateAnalysisOverview::runPass(Design* design) {
  using namespace std;

  ofstream f(this->analysis_overview.c_str());

  DataFlowGraph* dfg = design->getDataFlowGraph();

  int n_cluster = 0;
  map<string, string> colors_map;
  foreach_r_(Partition* p, design->getPartitions()) {
    f << "digraph G {" << endl;
    f << "subgraph cluster" << n_cluster;
    n_cluster++;
    f << " { " << endl;
    f << " node[style=filled, color=white];" << endl;
    f << "resource [shape=plaintext, label = < " << endl;
    f << TABLE << endl;
    f << TR << TD << CTD << "BRAMs" << CTD << "LUTs (k)" << CTD << "FFs (k)" << CTD << "DSPs";
    f << CTD << "BW";
    f << CTD << "Par" << CTD << "Exec. Time" << ETD << ETR << endl;
    colors_map.clear();
    int i = 0;


    // available resources
    // TODO pull this in a function...
    ifstream filenode("./platform.txt");
    string ab, al, af, ad, abw;
    string line;
    getline(filenode, line);
    filenode>>ab>>al>>af>>ad>>abw;
    double Ab, Al, Af, Ad, Abw;
    istringstream(ab)>>Ab;
    istringstream(al)>>Al;
    istringstream(af)>>Af;
    istringstream(ad)>>Ad;
    istringstream(abw)>>Abw;

    f << TR << TD << "Available" << CTD << Ab;
    f << CTD << (int) (Al / 1000) << CTD;
    f << (int)(Af / 1000);
    f << CTD << ad << CTD << abw << CTD << "-" << CTD << "-" << ETD;
    f << ETR;

    stringstream node_decl;

    foreach_ (Configuration *c, p->getPartition()) {
      string color = colors[i++];
      f << "<TR>" << endl;
      f << TD << "<FONT COLOR=\"" + color + "\"> Configuration ";
      f << c->getName() << "</FONT>" << ETD;
      f << TD << c->BRAMs << ETD;
      f << TD << (int)c->LUTs / 1000 << ETD;
      f << TD << (int)c->FFs / 1000 << ETD;
      f << TD << c->DSPs << ETD;
      f << TD << c->bandwidth;
      f << CTD << c->P << ETD;
      f << TD << c->getexecutionTime() << ETD;
      f << "</TR>" << endl;

      node_decl << "subgraph cluster" << n_cluster + i << " { " << endl;
      node_decl << " node[style=filled, color=white];" << endl;
      node_decl << "style = filled;\n color=lightgrey;\n fontsize=15; " << endl;
      node_decl << "label = \" Configuration " << c->getName() << "\";" << endl;
      foreach_(Segment* s, c->getConfiguration()) {
        foreach_ (DfeTask* d, s->getDfeTasks()) {
          colors_map[d->getName()] = color;
          string taskName = d->getName();
          node_decl << taskName << "[label = \"";
          node_decl << taskName;
          node_decl << "\", fontcolor=\"" + colors_map[taskName];
          node_decl << "\"];\n";
        }
      }
      node_decl<< "}\n";
    }

    f << TR << TD << "TOTAL" << ETD << endl;
    f << TD << CTD << CTD << CTD << CTD << CTD << CTD << endl;
    f << p->getexecutionTime() << ETD;
    f << ETR;

    f << "</TABLE>" << endl;
    f << ">];" << endl;
    f << "style = filled;\n color=lightgrey;\n fontsize=20; " << endl;
    f << "label = \"Partition " << p->getName() << "\";" << endl;
    f << "}" << endl;

    f << node_decl.str();

    DotPrint::writeDotEdges(f, dfg);
    f << "}" << endl;
  }

  f.close();

  using namespace boost::filesystem;
  path file_path(analysis_overview);
  cout << "    Generated analysis overview in " << file_path.string() << endl;
}
