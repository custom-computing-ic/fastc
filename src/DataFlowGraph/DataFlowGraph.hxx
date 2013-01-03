#ifndef DATAFLOWGRAPH_HXX_
#define DATAFLOWGRAPH_HXX_

#include <list>
#include "Node.hxx"

using namespace std;

class DataFlowGraph {

  Node *source, *sink;
  list<Node *> nodes;
  list<Node *> sources;

public:

  DataFlowGraph();
  void addInputNode(Node*);
  void addOutputNode(Node*);
  Node* addSource(Node*);
  void removeSource(Node *);
  Node* findNode(string name);
  Node* addNode(Node *n);
  string getDotRepresentation();
  Node* getSource();
  list<Node *> getSources();
  list<Node *> getNodes();

};

#endif /* DATAFLOWGRAPH_HXX_ */
