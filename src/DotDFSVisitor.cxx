#include "DotDFSVisitor.hxx"

#include <fstream>

void DotDFSVisitor::visit(Node *node) {
  foreach_ (Node* child, node->getNeighbours()) {
    dot += node->toDot() + " -> ";
    if (child != NULL)
      dot += child->toDot() + ";\n";
  }
}

void DotDFSVisitor::beforeVisit() {}

void DotDFSVisitor::afterVisit() {

}

string DotDFSVisitor::getDot() {
  return dot;
}

void DotDFSVisitor::writeDotToFile(ofstream &f) {
  f << "digraph { \n " << dot << "}";
}

void DotDFSVisitor::writeDotEdges(ofstream &f) {
  f << dot;
}
