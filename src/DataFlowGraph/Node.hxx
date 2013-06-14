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

public:
  Node(string name);
  string getName() const {return name;};
  bool operator<(Node& node) const;
  Node& operator=(const Node& node);
  //  friend ostream& operator<< (ostream &out, const Node& node);
  ostream& operator<<(ostream&);
  void addNeighbour(Node* node);
  string toDot();
  list<Node *> getNeighbours(){
    return neighbours;
  }
  string getId();
  virtual string toMaxJ()=0;
  virtual string getType() {return "hwFloat(8, 24)";};

};

class StreamOffsetNode : public Node {
public:
  StreamOffsetNode(string name) : Node(name) {};
  virtual string toMaxJ() {
    return "stream.offset(" + name + ", )\n";
  }
};

class OpNode : public Node {

public:
  OpNode(string op) : Node(op) {};
  string toMaxJ() {return "OpNode\n";}
};

class ExpressionNode : public Node {
public:
  ExpressionNode(string val) : Node(val) {}
  string* getExpr() {
    return &name;
  }
  string toMaxJ() {return "ExpressionNode";}
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

};

class ConstantNode : public Node {
  string val;
public:
  ConstantNode(string val) : Node(val) {}
  string toMaxJ() {return "constant.var(" + getType() + "," + getName() +")";}
};

class VarNode : public Node {
  string val;
public:
  VarNode(string val) : Node(val) {}
  string toMaxJ() {return "VarNode\n";}
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
};

#endif
