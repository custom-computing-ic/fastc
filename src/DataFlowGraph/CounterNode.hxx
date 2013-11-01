#ifndef COUNTERNODE_HXX_
#define COUNTERNODE_HXX_

#include "Node.hxx"
#include <string>

class CounterNode : public Node {

  std::string name;
  Node *max, *inc, *enable;

public:

  CounterNode(string name, Node *max, Node *inc, Node *enable) : Node(name) {
    this->max = max;
    this->inc = inc;
    this->enable = enable;
  }

  ostream& operator<<(ostream& out);
  string toMaxJ() {return "CounterNode\n";}
  string classname() {return "CounterNode";}  
  
};


#endif /*  */
