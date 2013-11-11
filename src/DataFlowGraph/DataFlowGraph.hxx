#ifndef DATAFLOWGRAPH_HXX_
#define DATAFLOWGRAPH_HXX_

#include <list>
#include "Node.hxx"

class DataFlowGraph {

  //if would be more general if multiple sinks can also be supported
  //list<Node *> sinks; 

public:
  Node *source, *sink;
  std::list<Node *>  nodes;
  std::list<Node *>  sources;
  std::list<Offset*> streams;
  std::list<Node*>   arithmetics;

  DataFlowGraph();
  void   addInputNode(Node*);
  void   addOutputNode(Node*);
  Node*  addSource(Node*);
  Node*  addNode(Node *n);
  
  void   removeSource(Node *);
  
  Node*  findNode(string name);

  Offset* findOffset(string name);
  Offset* addOffset(Offset* offset);
  
  Node* addArith(Node* opnode);
  
  string getDotRepresentation();
  
  Node*        getSource();
  std::list<Node *> getSources();
  std::list<Node *> getNodes();

};

#endif /* DATAFLOWGRAPH_HXX_ */
