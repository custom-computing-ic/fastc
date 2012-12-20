#ifndef OUTPUTNODE_HXX
#define OUTPUTNODE_HXX

#include "Node.hxx"

class OutputNode : public Node {

public:

  OutputNode(string name) : Node(name) {}
  string toMaxJ() {
    return "io.output(\"" + getName() + "\", " + getInput() + ", " + getType() + ")";
  }

  string getInput() {return getName();}
 
};

#endif
