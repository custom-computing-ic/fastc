#include "DataFlowGraph.hxx"
#include "SourceNode.hxx"
#include "SinkNode.hxx"

DataFlowGraph::DataFlowGraph() {
  source = addSource(new SourceNode("source"));
  sink = addNode(new SinkNode("sink"));
}

void DataFlowGraph::addInputNode(Node* n) {
  source->addNeighbour(n);
  nodes.push_front(n);
}

void DataFlowGraph::addOutputNode(Node *n) {
  n->addNeighbour(sink);
  nodes.push_front(n);
}

Node* DataFlowGraph::addSource(Node *n) {
  addNode(n);
  sources.push_front(n);
  return n;
}

void DataFlowGraph::removeSource(Node *n) {
  list<Node *>::iterator it;
  for (it = sources.begin(); it != sources.end(); it++)
    if ((*it)->getName().compare(n->getName()) == 0) {
      sources.erase(it);
      break;
    }
}

Node* DataFlowGraph::findNode(string name) {
  list<Node *>::iterator it;
  for (it = nodes.begin(); it != nodes.end(); it++) {
    if ((*it)->getName().compare(name) == 0)
      return *it;
  }
  return NULL;
}

Offset* DataFlowGraph::findOffset(string name) {
  list<Offset *>::iterator it;
  for (it = offsets.begin(); it != offsets.end(); it++) {
    if ((*it)->getName().compare(name) == 0)
      return *it;
  }
  return NULL;
}

Offset* DataFlowGraph::addOffset(Offset *offset) {
  offsets.push_front(offset);
  return offset;
}

Node* DataFlowGraph::addNode(Node *n) {
  nodes.push_front(n);
  return n;
}

string DataFlowGraph::getDotRepresentation() {
  return "digraph g {\n" + source->toDot() + "}";
}

Node* DataFlowGraph::getSource() {
  return source;
}

list<Node *> DataFlowGraph::getSources() {
  list<Node *> nodes_copy(sources.begin(), sources.end());
  return nodes_copy;
}

list<Node *> DataFlowGraph::getNodes() {
  list<Node *> nodes_copy(nodes.begin(), nodes.end());
  return nodes_copy;
}
