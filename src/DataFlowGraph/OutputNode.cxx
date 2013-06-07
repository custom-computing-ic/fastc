#include "OutputNode.hxx"
#include "../AstToMaxJVisitor.hxx"
#include "../utils.hxx"

string OutputNode::toMaxJ() {
  return "io.output(\"" + getName() + "\", " + getInput() + ", " + getType() + ");\n";
}

string OutputNode::getInput() {
  ASTtoMaxJVisitor visitor;
  visitor.traverse(node);
  return visitor.getSource();
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
  return strm << "hello";
}
