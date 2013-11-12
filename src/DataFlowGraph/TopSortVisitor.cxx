#include "TopSortVisitor.hxx"

bool TopSortVisitor::seenNode(Node* n) {
  foreach_ (Node* child, seenNodes) {
    if (n->getId().compare(child->getId()) == 0)
      return true;
  }
  return false;
}

TopSortVisitor::TopSortVisitor(DataFlowGraph *dfg) {
  this->dfg = dfg;
  seen = 0;
  foreach_(Node* node, dfg->getNodes())
    foreach_(Node* output, node->getNeighbours())
      output->addInput(node);//add this node as inputs for its neighbours
  
  foreach_(Node* node, dfg->getNodes())
    if (node->getName() != "sink" && node->getName() != "source")
      nodes.push_back(node);
}

void TopSortVisitor::sort(Node* n){
  if (!seenNode(n)) {
    foreach_ (Node* node, n->getInputs()) 
      sort(node);
   seenNodes.push_back(n);
  }
}

void TopSortVisitor::traverse() {
  beforeVisit();

  foreach_ (Node* node, nodes) {
    queue.push_back(node);
  }

  while (!queue.empty()) {
    Node* n = queue.back(); queue.pop_back();
    sort(n);
  }

  foreach_(Node* node, seenNodes)
    visit(node);
    //cout<<"sorted nodes: "<<node->getName()<<endl;

  afterVisit();
}
