#ifndef DATAFLOWGRAPH_HXX_
#define DATAFLOWGRAPH_HXX_

#include <list>
#include "Node.hxx"

using namespace std;

class DataFlowGraph {

  Node *source, *sink;
  list<Node *>  nodes;
  list<Node *>  sources;
  list<Offset*> offsets;
  list<OpNode*> arithmetics;
  //if would be more general if multiple sinks can also be supported
  //list<Node *> sinks; 

public:

  DataFlowGraph();
  void   addInputNode(Node*);
  void   addOutputNode(Node*);
  Node*  addSource(Node*);
  Node*  addNode(Node *n);
  
  void   removeSource(Node *);
  
  Node*  findNode(string name);

  Offset* findOffset(string name);
  Offset* addOffset(Offset* offset);
  
  string getDotRepresentation();
  
  Node*        getSource();
  list<Node *> getSources();
  list<Node *> getNodes();

};

#endif /* DATAFLOWGRAPH_HXX_ */
