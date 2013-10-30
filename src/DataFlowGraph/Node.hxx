#ifndef NODE_HXX
#define NODE_HXX

#include <string>
#include <list>
#include <iostream>

using namespace std;

class Node {

protected:
  static int s_idCount;
  int id;
  string name;
  list<Node *> neighbours;
  list<Node *> inputs;
  bool floating;
  int precision[2];

public:
  Node(string name);
  string getName() const {return name;};
  bool operator<(Node& node) const;
  Node& operator=(const Node& node);
  //  friend ostream& operator<< (ostream &out, const Node& node);
  ostream& operator<<(ostream&);
  void addNeighbour(Node* node);
  void addInput(Node* node);
  string toDot();
  list<Node *> getNeighbours(){
    return neighbours;
  }
  string getId();
  virtual string toMaxJ()=0;
  virtual string getType() {return "hwFloat(8, 24)";};
  virtual string classname() {return "Node";}
};

class Offset: public Node{
  
  list<string> offsets; 
  
  public:
  Offset(string name) : Node(name) {};
  void addoffset(string offset);
  int memory();
};


class StreamOffsetNode : public Node {
public:
  StreamOffsetNode(string name) : Node(name) {};
  virtual string toMaxJ() {
    return "stream.offset(" + name + ", )\n";
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

class MUXNode : public Node {
  Node *falseNode;
  Node *trueNode;
public:
  MUXNode(string name) : Node(name) {}
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

class ConstantNode : public Node {
  string val;
public:
  ConstantNode(string val) : Node(val) {}
  string toMaxJ() {return "constant.var(" + getType() + "," + getName() +")";}
  string classname() {return "ConstantNode";}
};

class VarNode : public Node {
  string val;
public:
  VarNode(string val) : Node(val) {}
  string toMaxJ() {return "VarNode\n";}
  string classname() {return "VarNode";}
};

class CounterNode : public Node {
  string limit, parent;
  int inc;
public:
  ostream& operator<<(ostream& out) {
	  out << "(" << name << ", wrap: " << limit;
	  out << ", inc: " << inc << ",parent: " << parent << ")";
	  return out;
  }
  CounterNode(string name, string limit, int inc) : Node(name) {
    this->limit = limit;
    this->inc   = inc;
  }
  CounterNode(string name, string limit, int inc, string parent) : Node(name) {
    this->limit = limit;
    this->inc   = inc;
    this->parent = parent;
  }
  string toMaxJ() {return "CounterNode\n";}
  string classname() {return "CounterNode";}
};

#endif
