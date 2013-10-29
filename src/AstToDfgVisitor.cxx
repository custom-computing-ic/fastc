#include "precompiled.hxx"
#include "utils.hxx"
#include "AstToDfgVisitor.hxx"

#include "DataFlowGraph/Node.hxx"

using namespace std;
using namespace boost;

ASTtoDFGVisitor :: ASTtoDFGVisitor(Kernel *k) {
  this->kernel = k;
  this->dfg = k->getDataFlowGraph();
}

void ASTtoDFGVisitor :: visit(SgNode *n) {

  if (isSgVariableDeclaration(n)) {
    SgVariableDeclaration *varDecl = isSgVariableDeclaration(n);

    foreach_(SgInitializedName *v, varDecl->get_variables()) {
      string variableName = v->get_qualified_name();

      SgInitializer *init = v->get_initializer();
      SgAssignInitializer *ass =  isSgAssignInitializer(init);
      if ( ass == NULL ) continue;

      SgExpression *exp = ass->get_operand();

      // i = count () ..
      SgFunctionCallExp *fcall = isSgFunctionCallExp(exp);
      if (fcall != NULL) {
        function_call_initializer(variableName, fcall);
      }

      // i = expr;
      Node* n = toExprNode(exp);
      if ( n != NULL ) {
        Node* varNode = new VarNode(variableName);
        dfg->addNode(n);
        dfg->addNode(varNode);
        n->addNeighbour(varNode);
      }
    }

  } else if (isSgFunctionCallExp(n)) {
    SgFunctionCallExp *fcall = isSgFunctionCallExp(n);
    SgExpressionPtrList args = fcall->get_args()->get_expressions();
    SgExpressionPtrList::iterator it = args.begin();
    SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
    string fname = fsymbol->get_name();

    // TODO handle counter functions
    /* if (fname.compare("output") == 0) {
      Node *outputNode = toNode(*it);
      Node *result     = toNode(*(++it));
      if (result != NULL)
        result->addNeighbour(outputNode);
        } */
  } else if (isSgExprStatement(n)) {
    toExprNode(isSgExprStatement(n)->get_expression());
  }

}


void ASTtoDFGVisitor :: function_call_initializer(string& variableName, SgFunctionCallExp *fcall) {
  SgExpressionPtrList args = fcall->get_args()->get_expressions();
  SgExpressionPtrList::iterator itt;
  itt=args.begin();
  SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
  string fname = fsymbol->get_name();
  if (fname.compare("count") == 0 || fname.compare("countChain") == 0) {
    SgVarRefExp *dim = isSgVarRefExp(*itt);
    string d = "128";
    if (dim != NULL)
      d = dim->get_symbol()->get_name();

    SgIntVal *inc = isSgIntVal(*(++itt));
    int i;
    if (inc != NULL) {
      i = inc->get_value();
    }

    if (fname.compare("count") == 0) {
      CounterNode *counterNode = new CounterNode(variableName, d, i);
      dfg->addSource(counterNode);
    } else if (fname.compare("countChain") == 0) {

      string p = "";
      SgVarRefExp *parent = isSgVarRefExp(*(++itt));
      if (parent != NULL)
        p = parent->get_symbol()->get_name();

      CounterNode *counterNode = new CounterNode(variableName, d, i, p);

      Node *childNode = dfg->findNode(p);
      if ( childNode != NULL ) {
        counterNode->addNeighbour(childNode);
        dfg->removeSource(childNode);
      }

      dfg->addSource(counterNode);
    }
  } else if (fname.compare("fselect") == 0) {

    MUXNode *muxNode = new MUXNode(variableName);
    dfg->addNode(muxNode);

    SgExpression *condExp = isSgExpression(*itt);
    Node *exp = toExprNode(*itt);
    //dfg->addSource(exp);
    if (exp != NULL)
      exp->addNeighbour(muxNode);

    Node *n = toNode(*(++itt));
    if (n != NULL)
      muxNode->setIfTrueNode(n);

    Node *n2 = toNode(*(++itt));
    if (n2 != NULL)
      muxNode->setIfFalseNode(n2);
  }

}

Node* ASTtoDFGVisitor :: toNode(SgExpression *ex) {

  SgIntVal *intVal = isSgIntVal(ex);

  if (intVal != NULL) {
    stringstream out;
    out << intVal->get_value();
    Node *n = new ConstantNode(out.str());
    dfg->addSource(n);
    return n;
  }

  SgVarRefExp *var = isSgVarRefExp(ex);
  if (var != NULL)
    return dfg->findNode(var->get_symbol()->get_name());

  return NULL;
}

Node* ASTtoDFGVisitor :: toExprNode(SgExpression *ex) {
  return toExprNodeRec(ex);
}

Node* ASTtoDFGVisitor :: toExprNodeRec(SgExpression *ex) {
  if (isSgVarRefExp(ex) ) {
    string name = isSgVarRefExp(ex)->get_symbol()->get_name();
    return dfg->findNode(name);
  } else if (isSgIntVal(ex) )  {
    SgIntVal *e = isSgIntVal(ex);
    string val = e->unparseToString();
    Node *n = dfg->findNode(val);
    if (n == NULL) {
      n = new ConstantNode(val);
      dfg->addSource(n);
    }
    return n;
  } else if (isSgBinaryOp(ex)) {

    SgBinaryOp *e = isSgBinaryOp(ex);

    Node *right = toExprNodeRec(e->get_rhs_operand());

    if (isSgAssignOp(e)) {

      // assignments are a bit special
      if (isSgPntrArrRefExp(e->get_lhs_operand())) {
        // handles: y[offset_expression] = expression
        Node* offset_node = new StreamOffsetNode("Offset");

        SgPntrArrRefExp *ee = isSgPntrArrRefExp(e->get_lhs_operand());
        string name = ee->get_lhs_operand()->unparseToString();

        // construct DFG for the offset expression, which should
        // feed into the offset node
        Node *offset_expr = toExprNodeRec(ee->get_rhs_operand());
        offset_expr->addNeighbour(offset_node);

        // node corresponding to y
        Node* stream_node = dfg->findNode(name);

        // RHS should feed into the offset node
        right->addNeighbour(offset_node);

        // the offset node should feed into the LHS stream
        if (stream_node != NULL)
          offset_node->addNeighbour(stream_node);
        else {
          cout << "Attempting to add a null node at line 196: ";
          cout << "Could not find dfg node for name: " + name << endl;
        }

        return stream_node;
      } else if (isSgVarRefExp(e->get_lhs_operand())) {
        // handles: y = expression
        string name = e->get_lhs_operand()->unparseToString();
        Node* var_node = dfg->findNode(name);
        if (var_node == NULL) {
          var_node = new VarNode(name);
          dfg->addNode(var_node);
        }
        right->addNeighbour(var_node);
        return var_node;
      }

    }


    string op;
    if (isSgAddOp(e))
      op = "+";
    else if (isSgMultiplyOp(e))
      op = "*";
    else if (isSgAndOp(e) || isSgBitAndOp(e) )
      op = "&";
    else if (isSgLessThanOp(e))
      op = "<";
    else if (isSgGreaterThanOp(e))
      op = ">";

    Node *left = toExprNodeRec(e->get_lhs_operand());
    Node *node;
    if ( isSgPntrArrRefExp(ex))
      node = new StreamOffsetNode("Offset");
    else
      node = new OpNode(op);

    dfg->addNode(node);
    if (right != NULL)
      right->addNeighbour(node);
    if (left != NULL)
      left->addNeighbour(node);

    return node;
  }

  if (isSgUnaryOp(ex)) {
    SgUnaryOp *unOp = isSgUnaryOp(ex);
    Node *n = toExprNodeRec(unOp->get_operand());
    string op;
    if (isSgMinusOp(ex))
      op = "-";
    OpNode *node = new OpNode(op);
    n->addNeighbour(node);
    dfg->addNode(node);
    return node;
  }

  D(cerr << "Couldn't create node for: " + ex->unparseToString() << endl);
  return NULL;
}

void ASTtoDFGVisitor :: atTraversalEnd() {}
