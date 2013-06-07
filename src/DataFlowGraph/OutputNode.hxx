#ifndef OUTPUTNODE_HXX
#define OUTPUTNODE_HXX

#include "Node.hxx"
#include "../precompiled.hxx"

class OutputNode : public Node {

  SgNode* node;

public:

  OutputNode(string name, string type, string width) : Node(name){}

  string toMaxJ();
  string getInput();
  friend std::ostream& operator<<(std::ostream&, const OutputNode&);
  bool operator==(const OutputNode&);
  void setSgNode(SgNode*);
};

#endif
