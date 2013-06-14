#ifndef OUTPUTNODE_HXX
#define OUTPUTNODE_HXX

#include "Node.hxx"
#include "../precompiled.hxx"
//#include "../Kernel.hxx"

class Kernel;

class OutputNode : public Node {

  SgNode* node;
  string width;
  string ioType, computeType;
  Kernel* kernel;

public:

  OutputNode(Kernel* kernel, string name, string ioType, string computeType, string width) : Node(name){
    this->width = width;
    this->ioType = ioType;
    node = NULL;
    this->kernel = kernel;
    this->computeType = computeType;
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
    if (width != "1")
      return "new KArrayType<HWVar> (" + ioType + ", " + width + ")";
    else
    return ioType;
  }
};

#endif
