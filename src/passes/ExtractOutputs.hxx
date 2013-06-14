#ifndef EXTRACT_OUTPUTS_HXX
#define EXTRACT_OUTPUTS_HXX

#include "../precompiled.hxx"
#include "../DataFlowGraph/OutputNode.hxx"

class ExtractOutputs : public AstSimpleProcessing
{

  list<OutputNode*> kernelOutputs;

public:
  ExtractOutputs(list<OutputNode*> kernelOutputs) {
    this->kernelOutputs = kernelOutputs;
  }

  virtual void visit(SgNode *node) {

    SgExprStatement* stmt;
    if ((stmt = isSgExprStatement(node)) != NULL ) {
      SgExpression* expr = stmt->get_expression();
      SgAssignOp* assign;
      if ( (assign = isSgAssignOp(expr)) != NULL ) {
        SgExpression* lhs = assign->get_lhs_operand();
        SgExpression* nameExp;
        if (SageInterface::isArrayReference(assign->get_lhs_operand(), &nameExp)) {
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
      }
    }
  }

};
#endif
