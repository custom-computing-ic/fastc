#ifndef SINKNODE_HXX_
#define SINKNODE_HXX_

#include <string>
#include "Node.hxx"

using namespace std;

class SinkNode: public Node {
public:
	SinkNode(string name) :
			Node(name) {
	}
	;
	string toMaxJ();
};

#endif /* SINKNODE_HXX_ */
