#include "DFSVisitor.hxx"

bool DFSVisitor::seenNode(Node* n) {
  list<Node *>::iterator it;
  for (it = seenNodes.begin(); it != seenNodes.end(); it++) {
    if (n->getId().compare((*it)->getId()) == 0)
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

  list<Node *>::iterator it;
  for (it = nodes.begin(); it != nodes.end(); it++) {
    queue.push_front(*it);
  }

  // TODO does not handle cycles
  // TODO does not handle multiple connected comps
  while (!queue.empty()) {
    Node* n = queue.front();
    queue.pop_front();
    if (!seenNode(n)) {
      seen++;
      visit(n);
      list<Node *> neighbours = n->getNeighbours();
      for (it = neighbours.begin(); it != neighbours.end(); it++)
        queue.push_front(*it);
    }

    seenNodes.push_front(n);
  }

  afterVisit();
}
