#include "../precompiled.hxx"
#include "Node.hxx"

#include <iostream>
#include <typeinfo>

using namespace std;
using namespace boost;

Node::Node(string name) {
  this->id = s_idCount++;
  this->name = name;
  //TODO: support for fixed point
  this->floating = true;
  //TODO: dynamic transformation ratio
  this->transformation = 0;

  //TODO: precision analysis
  //currently the precision is fixed to be 8-10 floating
  this->precision[0] = 8;
  this->precision[1] = 24;
}

ostream& Node::operator<< (ostream &out) {
  out << name << "[ " ;
  foreach_ (Node* n, neighbours) {
    out << n->getName() << " ";
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
