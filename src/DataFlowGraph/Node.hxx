#ifndef NODE_HXX
#define NODE_HXX

#include <string>
#include <list>
#include <iostream>

#include "../StencilOffset.h"
#include "Utils.h"

using namespace std;

class Node {

protected:
  static int s_idCount;
  int id;
  string name;
  list<Node *> neighbours;

public:
  list<Node *> inputs;
  bool floating;
  int transformation;
  int precision[2];
  Node(string name="");
  virtual string getName() const {return name;};
  bool operator<(Node& node) const;
  Node& operator=(const Node& node);
  ostream& operator<<(ostream&);
  void addNeighbour(Node* node);
  void addInput(Node* node);
  virtual string toDot();
  list<Node *> getNeighbours(){
    return neighbours;
  }
  list<Node *> getInputs(){
    return inputs;
  }
  string getId();
  virtual string toMaxJ()=0;
  virtual string getType() {return "hwFloat(8, 24)";};
  virtual string classname() {return "Node";}
};

class Offset : public Node{

  StencilOffset* stencilOffset;

public:
  list<string> offsets;
  list<int> OnchipMemory;
  int delay;//calculated in HLAVisitor
  int inputdelay;//updated in DfeTopSortVisitor

  public:
  Offset(string name) : Node(name) {delay =0; inputdelay=0;};
  void addoffset(string offset);
  void setDelay(int inputdelay) {this->inputdelay = inputdelay;}
  int getDelay() {return this->inputdelay;}
  int memory();
  string toMaxJ() {return "Offset\n";}
  string classname() {return "Offset";}
  void setStencilOffset(StencilOffset* stencilOffset) {
    // TODO do some useful calculation
    this->stencilOffset = stencilOffset;
  }
};


class StreamOffsetNode : public Node {
public:
  StencilOffset* stencilOffset;
  StreamOffsetNode(string name) : Node(name) {
    stencilOffset = NULL;
  };
  virtual string toMaxJ() {
    return "stream.offset(" + name + ", )\n";
  }
  void setStencilOffset(StencilOffset* stencilOffset) {
    this->stencilOffset = stencilOffset;
  }
  string toDot() {
    string dot = "Offset__" + getId() + "__offsets__";

    if (stencilOffset == NULL)
      return dot;

    map<string, int>::iterator it;

    if (stencilOffset->var_offset.size() > 0) {
      it = stencilOffset->var_offset.begin();
      if ( it != stencilOffset->var_offset.end()) {
	for (it = stencilOffset->var_offset.begin(); it != stencilOffset->var_offset.end(); it++) {
	  string snd = boost::lexical_cast<string>(abs(it->second));
	  if (it->second < 0)
	    snd = "neg" + snd;
	  dot += it->first + "_" + snd + "__";
	}
      }
    }

    dot += "__dims__";
    if (stencilOffset->dim_offset.size() > 0) {
      if (stencilOffset->dim_offset.size() > 0) {
	for (it = stencilOffset->dim_offset.begin();
	     it != stencilOffset->dim_offset.end();
	     it++) {
	  string snd = boost::lexical_cast<string>(abs(it->second));
	  if (it->second < 0)
	    snd = "neg" + snd;
	  dot += it->first + "_" + snd + "__";
	}
      }
    }
    return  dot;
  }

};

class OpNode : public Node {//arithme node

public:
  OpNode(string op) : Node(op) {};
  int resource(int transformation);

  string toMaxJ() {return "OpNode\n";}
  string classname() {return "OpNode";}
};

class ExpressionNode : public Node {
public:
  ExpressionNode(string val) : Node(val) {}
  string* getExpr() {
    return &name;
  }
  string toMaxJ() {return "ExpressionNode";}
  string classname() {return "ExpressionNode";}
};


class VarNode : public Node {
  string val;
public:
  VarNode(string val) : Node(val) {}
  string toMaxJ() {return "VarNode\n";}
  string classname() {return "VarNode";}
};


class ConstantNode : public Node {
  string val;
public:
  ConstantNode(string val) : Node(val) {}
  string toMaxJ() {return "constant.var(" + getType() + "," + getName() +")";}
  string classname() {return "ConstantNode";}
};

#endif
