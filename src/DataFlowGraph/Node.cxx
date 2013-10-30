#include "../precompiled.hxx"
#include "Node.hxx"


#include <iostream>
#include <iterator>
#include <sstream>
#include <typeinfo>

using namespace std;
using namespace boost;

Node::Node(string name) {
  this->id = s_idCount++;
  this->name = name;
}

ostream& Node::operator<< (ostream &out) {
  out << name << "[ " ;

  list<Node *>::iterator it;
  string dot;
  for (it = neighbours.begin(); it != neighbours.end(); it++) {
    out << (*it)->getName() << " ";
  }

  out << "]";
  return out;
}

bool Node::operator<(Node& node) const{
  return name < node.getName();
}

Node& Node::operator=(const Node& node){
  name = node.name;
  return *this;
}

void Node::addNeighbour(Node *node) {
  neighbours.push_front(node);
}

void Node::addInput(Node *node) {
  inputs.push_front(node);
}

string Node::toDot() {
  return "\"" + name + "[id: " + getId() + "]" + classname() + "\"";
}

string Node::getId() {
  return lexical_cast<string>(id);
}

int Node::s_idCount = 0;
