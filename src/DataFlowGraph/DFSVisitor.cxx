#include "DFSVisitor.hxx"

bool DFSVisitor::seenNode(Node* n) {
  foreach_ (Node* child, seenNodes) {
    if (n->getId().compare(child->getId()) == 0)
      return true;
  }
  return false;
}

DFSVisitor::DFSVisitor(DataFlowGraph *dfg) {
  this->dfg = dfg;
  seen = 0;
  nodes = dfg->getSources();
}

void DFSVisitor::traverse() {
  beforeVisit();

  foreach_ (Node* node, nodes) {
    queue.push_front(node);
  }

  // TODO does not handle cycles
  // TODO does not handle multiple connected comps
  while (!queue.empty()) {
    Node* n = queue.front(); queue.pop_front();
    if (!seenNode(n)) {
      seen++;
      visit(n);
      foreach_ (Node* node, n->getNeighbours()) {
        queue.push_front(node);
      }
    }
    seenNodes.push_front(n);
  }

  afterVisit();
}
