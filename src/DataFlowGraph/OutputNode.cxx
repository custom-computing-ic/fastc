#include "OutputNode.hxx"
#include "../AstToMaxJVisitor.hxx"
#include "../utils.hxx"

string OutputNode::toMaxJ() {
  return "io.output(\"" + getName() + "\", " + getInput() + ", " + getType() + ");\n";
}

string OutputNode::getInput() const {
  if (node != NULL) {
    cout << "Getting input for node " + getName() << endl;
    cout << "Expression " << node->unparseToString() << endl;
    ASTtoMaxJVisitor visitor(kernel);
    visitor.traverse(node);
    cout << "Got input " << visitor.getSource() << endl;
    return visitor.getSource();
  }
  else return "ERROR Processing Output Node";
}

bool OutputNode::operator==(const OutputNode& other) {
  return other.getName() == getName();
}

void OutputNode::setSgNode(SgNode* node) {
  SgExpression* expr;
  if ((expr = isSgExpression(node)) != NULL)
    this->node = node;
  else
    cerr << "Error: setting SgNode, must be SgExpression" << endl;
}

std::ostream& operator<<(std::ostream &strm, const OutputNode& node) {
  strm << "[" << node.getName() << ", ";
  strm << node.getInput() << "]";
  return strm;
}
