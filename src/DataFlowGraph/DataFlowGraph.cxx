#ifndef DATAFLOWGRAPH_CXX
#define DATAFLOWGRAPH_CXX
#include "Node.hxx"


class SourceNode : public Node {
	string imports() {
		return "import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;\n"
		        "import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;\n"
		        "import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;";
	}

	string kernelName() {
		return "KernelFunc";
	}
public:
  SourceNode(string name) : Node(name){};
  string toMaxJ() {
	  // TODO how to handle different constructor?
	  return  "package source;\n\n" + imports() + "\n\n"
			  "public class" + kernelName() + "extends Kernel {\n\n"
	  	  	  "public" + kernelName() + "(KernelParameters parameters) {\n"
	  	  	  "super(parameters);";
  }

};

class SinkNode : public Node {
public:
  SinkNode(string name) : Node(name){};
  string toMaxJ() {return "sinkNode\n";}
};

class DataFlowGraph {

  Node *source, *sink;
  list<Node *> nodes;
  list<Node *> sources;

public:

  DataFlowGraph() {
    source = addSource(new SourceNode("source"));
    sink   = addNode(new SinkNode("sink"));

  };

  void addInputNode(Node* n) {
    source->addNeighbour(n);
    nodes.push_front(n);
  }

  void addOutputNode(Node *n) {
    n->addNeighbour(sink);
    nodes.push_front(n);
  }

  Node* addSource(Node *n) {
    addNode(n);
    sources.push_front(n);
    return n;
  }

  void removeSource(Node *n) {
    list<Node *>::iterator it;
    for (it = sources.begin(); it != sources.end(); it++)
      if ( (*it)->getName().compare(n->getName()) == 0 ) {
	sources.erase(it);
      break;
      }
  }

  Node* findNode(string name) {
    list<Node *>::iterator it;
    for (it = nodes.begin(); it != nodes.end(); it++) {
      if ( (*it)->getName().compare(name) == 0 )
	return *it;
    }
    return NULL;
  }

  Node* addNode(Node *n) {
    nodes.push_front(n);
    return n;
  }

  string getDotRepresentation() {
    return "digraph g {\n" + source->toDot() + "}";
  }

  Node* getSource() {
    return source;
  }

  list<Node *> getSources() {
    list<Node *> nodes_copy(sources.begin(), sources.end());
    return nodes_copy;
  }

  list<Node *> getNodes() {
    list<Node *> nodes_copy(nodes.begin(), nodes.end());
    return nodes_copy;
  }

};

#endif
