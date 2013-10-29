#ifndef SOURCENODE_HXX
#define SOURCENODE_HXX

#include <string>
#include "Node.hxx"

using namespace std;

class SourceNode: public Node {
  string imports();
  string kernelName();

public:
  SourceNode(string name) : Node(name) {};
  string toMaxJ();
  string classname() {return "SourceNode";}
};

#endif
