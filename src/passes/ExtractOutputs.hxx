#ifndef EXTRACT_OUTPUTS_HXX
#define EXTRACT_OUTPUTS_HXX

#include "../precompiled.hxx"
#include "../DataFlowGraph/OutputNode.hxx"

class ExtractOutputs : public AstSimpleProcessing
{

  list<OutputNode*> kernelOutputs;

public:
  ExtractOutputs(list<OutputNode*> kernelOutputs) {
    cout << "Building output assignment pass" << endl;
    this->kernelOutputs = kernelOutputs;
  }

  virtual void visit(SgNode *node) {

    SgExprStatement* stmt;
    if ((stmt = isSgExprStatement(node)) != NULL ) {
      SgExpression* expr = stmt->get_expression();

      SgAssignOp* assign;
      if ( (assign = isSgAssignOp(expr)) != NULL ) {
        cout << "Visting assign " << assign->unparseToString() << endl;
        SgExpression* lhs = assign->get_lhs_operand();
        cout << assign->get_lhs_operand()->unparseToString() << endl;
        SgPntrArrRefExp* lhsRef;
        if ( (lhsRef = isSgPntrArrRefExp(lhs)) != NULL) {
          string outName = lhsRef->get_lhs_operand()->unparseToString();
          cout << "Outputs size: " << kernelOutputs.size() << endl;

          foreach_(OutputNode* outNode, kernelOutputs) {
            cout << "Kernel Output " << *outNode << endl;
            if (outNode->getName() == outName) {
              //            node->setSgNode();
              cout << "Found output should set SgNode" << endl;
              cout << "Assign: " << assign->unparseToString() << endl;
              outNode->setSgNode(assign->get_rhs_operand());
              SageInterface::removeStatement((SgStatement *)node);
              cout << "Removed statement from AST " << endl;
            }
          }
        }
      }
    }
  }

};
#endif
