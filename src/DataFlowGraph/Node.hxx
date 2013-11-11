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
  string getId();
  virtual string toMaxJ()=0;
  virtual string getType() {return "hwFloat(8, 24)";};
  virtual string classname() {return "Node";}
};

class Offset : public Node{

  public:
  list<string> offsets;
  list<int> OnchipMemory;
  int delay;

  public:
  Offset(string name) : Node(name) {};
  void addoffset(string offset);
  int memory();
  string toMaxJ() {return "Offset\n";}
  string classname() {return "Offset";}
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
