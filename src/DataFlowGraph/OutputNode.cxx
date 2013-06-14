#include "OutputNode.hxx"
#include "../AstToMaxJVisitor.hxx"
#include "../utils.hxx"

string OutputNode::toMaxJ() {
  return "io.output(\"" + getName() + "\", " + getInput() + ", " + getType() + ");\n";
}

string OutputNode::getInput() const {
  if (node != NULL) {
    ASTtoMaxJVisitor visitor(kernel);
    visitor.traverse(node);
    string source = visitor.getSource();
    if ( computeType != ioType )
      source += ".cast(" + ioType + ")";
    return source;
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
