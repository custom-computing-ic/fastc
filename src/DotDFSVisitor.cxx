#include "DotDFSVisitor.hxx"

#include <fstream>

void DotDFSVisitor::visit(Node *node) {
  foreach_ (Node* child, node->getNeighbours()) {
    dot += node->toDot() + " -> ";
    if (child != NULL)
      dot += child->toDot() + ";\n";
  }
}

void DotDFSVisitor::beforeVisit() {
  dot += "digraph {\n";
}

void DotDFSVisitor::afterVisit() {
  dot += "}";
}

string DotDFSVisitor::getDot() {
  return dot;
}

void DotDFSVisitor::writeDotToFile(string fname) {
  ofstream f(fname.c_str());
  f << dot;
  f.close();
}
