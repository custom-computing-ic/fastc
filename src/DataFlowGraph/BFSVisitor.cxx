#ifndef BFSVISITOR_CXX
#define BFSVISITOR_CXX

#include <list>

class BFSVisitor {
  DataFlowGraph *dfg;
  list<Node*> queue, seenNodes;
  list<Node*> nodes;
  int seen;

  bool seenNode(Node* n) {
    list<Node*>::iterator it;
    for(it = seenNodes.begin(); it != seenNodes.end(); it++)
      if (n->getId().compare((*it)->getId()) == 0)
	return true;
    return false;
  }

public:
  BFSVisitor(DataFlowGraph *dfg) {
    this->dfg = dfg;
    seen = 0;
    nodes = dfg->getSources();
  }

  void traverse() {
    beforeVisit();
    
    list<Node *>:: iterator it;
    for (it = nodes.begin(); it != nodes.end(); it++) {
      //  cout << "Adding source " << (*it)->getName() << endl;
      queue.push_front(*it);  
    }

    
    // TODO does not handle cycles
    // TODO does not handle multiple connected comps
    while ( !queue.empty() ) {
      Node* n = queue.front(); queue.pop_front();
      if ( !seenNode(n) ) {
	seen++;
	visit(n);
	list<Node *> neighbours = n->getNeighbours();
	for(it=neighbours.begin(); it!=neighbours.end(); it++)
	  queue.push_back(*it);
      }

      seenNodes.push_front(n);      
    }
    //cout << "seen " << seen << "/" << dfg->getNodes().size() << endl;

    afterVisit();
  }

  virtual void visit(Node *n)=0;
  virtual void beforeVisit() {}
  virtual void afterVisit()  {}

};

#endif
