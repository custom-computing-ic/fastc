#include "CounterNode.hxx"

ostream& CounterNode::operator<<(ostream& out) {
	  out << "(" << name << ", wrap: " << max;
	  out << ", inc: " << inc << ",parent: " << enable << ")";
	  return out;
}
