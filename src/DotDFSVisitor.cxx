#include "DotDFSVisitor.hxx"

#include "DataFlowGraph/DFSVisitor.cxx"
#include "DataFlowGraph/DataFlowGraph.cxx"

#include <string>

void DotDFSVisitor::visit(Node *n) {
    list<Node *>::iterator it;
    list<Node *> neighbours = n->getNeighbours();
    for (it = neighbours.begin(); it != neighbours.end(); it++)
      dot += n->toDot() + " -> " + (*it)->toDot() + ";\n";
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
