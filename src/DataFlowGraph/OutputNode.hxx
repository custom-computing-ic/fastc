#ifndef OUTPUTNODE_HXX
#define OUTPUTNODE_HXX

#include "Node.hxx"
#include "../precompiled.hxx"
//#include "../Kernel.hxx"

class Kernel;

class OutputNode : public Node {

  SgNode* node;
  string width;
  string type;
  Kernel* kernel;

public:

  OutputNode(Kernel* kernel, string name, string type, string width) : Node(name){
    this->width = width;
    this->type = type;
    node = NULL;
    this->kernel = kernel;
  }

  string getWidth() {
    return width;
  }

  string toMaxJ();
  string getInput() const;
  friend std::ostream& operator<<(std::ostream&, const OutputNode&);
  bool operator==(const OutputNode&);
  void setSgNode(SgNode*);

  string getType() {
    return type;
  }
};

#endif
