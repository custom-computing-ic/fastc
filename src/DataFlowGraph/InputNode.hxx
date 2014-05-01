#ifndef INPUTNODE_HXX
#define INPUTNODE_HXX

#include "Node.hxx"

class InputNode : public Node {

public:

  InputNode(string name) : Node(name) {}
  string toMaxJ() {
    return 
      "HWVar " + getName() + 
      " = io.input(\"" + getName() + "\"," + getType() + ")";
  }
  
  string classname() { return "InputNode"; }
};

#endif
