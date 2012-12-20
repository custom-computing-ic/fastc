#include "Node.hxx"

#include <iostream>
#include <iterator>
#include <sstream>

using namespace std;

Node::Node(string name) {
  this->id = s_idCount++;
  this->name = name;
}

ostream& Node::operator<< (ostream &out) {
  out << name << "[ " ;

  list<Node *>::iterator it;
  string dot;
  for (it = neighbours.begin(); it != neighbours.end(); it++) {
    cout << (*it)->getName() << " ";
    }

  cout << "]";
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

string Node::toDot() {
  return "\"" + name + "[id: " + getId() + "]\"";
}

string Node::getId() {
  stringstream out;
  out << id;
  return out.str();
}

int Node::s_idCount = 0;
