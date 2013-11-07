#ifndef MUXNODE_HXX_
#define MUXNODE_HXX_

#include "Node.hxx"
#include <boost/lexical_cast.hpp>

class MuxNode : public Node {
  Node *falseNode;
  Node *trueNode;
public:
  MuxNode() : Node() {
    this->name = boost::lexical_cast<string>(s_idCount) + "MuxNode";
  }
  void setIfTrueNode(Node *n) {
    falseNode = n;
    n->addNeighbour(this);
  }
  void setIfFalseNode(Node *n) {
    trueNode = n;
    n->addNeighbour(this);
  }
  string toMaxJ() {return "MUXNode\n";}
  string classname() {return "MUXNode";}
};

#endif
