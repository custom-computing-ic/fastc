#include "ExtractOutputs.hxx"

ExtractOutputs::ExtractOutputs(list<OutputNode*> kernelOutputs) {
  this->kernelOutputs = kernelOutputs;
}

void ExtractOutputs::visit(SgNode *node) {
  SgExprStatement* stmt = isSgExprStatement(node);
  if (stmt == NULL )
    return;

  SgExpression* expr = stmt->get_expression();
  SgAssignOp* assign = isSgAssignOp(expr);
  if ( assign == NULL )
    return;

  SgExpression* lhs = assign->get_lhs_operand();
  SgExpression* nameExp;

  if (!SageInterface::isArrayReference(assign->get_lhs_operand(), &nameExp))
    return;

  string outName = nameExp->unparseToString();
  foreach_(OutputNode* outNode, kernelOutputs) {
    if (outNode->getName() == outName) {
      if (outNode->getWidth() != "1") {
        outNode->setSgNode(nameExp);
      } else {
        outNode->setSgNode(assign->get_rhs_operand());
        SageInterface::removeStatement((SgStatement *)node);
      }
    }
  }
}
