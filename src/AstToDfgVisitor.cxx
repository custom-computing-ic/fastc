#include "precompiled.hxx"
#include "utils.hxx"
#include "AstToDfgVisitor.hxx"

#include "DataFlowGraph/Node.hxx"
#include "DataFlowGraph/MuxNode.hxx"
#include "DataFlowGraph/CounterNode.hxx"

using namespace std;
using namespace boost;

ASTtoDFGVisitor :: ASTtoDFGVisitor(Kernel *k) {
  this->kernel = k;
  this->dfg = k->getDataFlowGraph();
}

void ASTtoDFGVisitor :: visit(SgNode *n) {

  if ( n == NULL || (n != NULL && n->getAttribute("ignore") != NULL) )
    return;

  if (isSgForStatement(n)) {
    visitFor(isSgForStatement(n));
  } else if (isSgVariableDeclaration(n)) {

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
    SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
    string fname = fsymbol->get_name();
  } else if (isSgExprStatement(n)) {
    toExprNode(isSgExprStatement(n)->get_expression());
  }
}

void ASTtoDFGVisitor::ignoreChildren(SgNode *n) {
  vector<SgNode *> children = n->get_traversalSuccessorContainer();
  ignore(n);
  foreach_(SgNode *n, children) {
    if (n != NULL)
      ignoreChildren(n);
  }
}

void ASTtoDFGVisitor::ignore(SgNode *n) {
  AstAttribute *ignore = new AstTextAttribute("yes");
  n->setAttribute("ignore", ignore);
}

void ASTtoDFGVisitor::visitFor(SgForStatement *forStmt) {

  // ignore everything encountered in a loop stmt for the purpose of
  // the DFG

  SgStatementPtrList initList = forStmt->get_init_stmt();
  SgStatement        *test = forStmt->get_test();
  SgExpression       *incr = forStmt->get_increment();

  foreach_(SgStatement* stmt, initList) {
    ignoreChildren(stmt);
  }
  ignoreChildren(test); ignoreChildren(incr);
}


void ASTtoDFGVisitor :: function_call_initializer(string& variableName, SgFunctionCallExp *fcall) {
  SgExpressionPtrList args = fcall->get_args()->get_expressions();
  SgExpressionPtrList::iterator itt = args.begin();
  SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
  string fname = fsymbol->get_name();

  if (fname == "count") {

    // extract width
    // TODO this can be an arbirary expression of compile time values
    Node* width = toExprNode(*itt);

    // extract maximum
    // TODO this can be an arbitrary expression of compile time values
    Node* max = toExprNode(*(++itt));

    // extract increment
    // TODO this can be an arbitrary expression of compile time values
    Node* inc = toExprNode(*(++itt));

    // extract parent name
    // TODO throw error if this is not a variable reference
    SgVarRefExp *parent = isSgVarRefExp(*(++itt));
    string enableName = parent == NULL ? NULL : parent->get_symbol()->get_name();
    Node *enable = dfg->findNode(enableName);
    CounterNode *counter = new CounterNode(variableName, max, inc, enable);

    if ( enable != NULL ) {
      enable->addNeighbour(counter);
    }

    dfg->addSource(counter);

  } else if (fname == "fselect") {


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


Node* ASTtoDFGVisitor::toNodePntrArrRef(SgPntrArrRefExp *ex) {
  SgPntrArrRefExp *ee = isSgPntrArrRefExp(ex);
  StreamOffsetNode* node = new StreamOffsetNode("Offset");

  string name = ee->get_lhs_operand()->unparseToString();
  Offset* offset = dfg->findOffset(name);
  if (offset == NULL) {
    offset = new Offset(name);
    offset->offsets.push_back(ee->get_rhs_operand()->unparseToString());
    dfg->addOffset(offset);
  }
  else
    offset->offsets.push_back(ee->get_rhs_operand()->unparseToString());
  D(cout<<"buffer "<<offset->getName()<<endl;)
  for(list<string>::iterator it = offset->offsets.begin(); it!=offset->offsets.end(); ++it)
    D(cout<<" "<< *it;)
  D(cout<<endl;)

  if (kernel->isStencilKernel()) {
    // extract stencil offsets from this expression
    StencilOffset* stencilOffset =
      StencilUtils::extractSingleOffset(ee->get_rhs_operand(),
                                        kernel->getFirstStencil());
    offset->addStencilOffset(stencilOffset);
    node->setStencilOffset(stencilOffset);
  }

  return node;
}

Node* ASTtoDFGVisitor :: toExprNodeRec(SgExpression *ex) {
  if (isSgVarRefExp(ex) ) {
    string name = isSgVarRefExp(ex)->get_symbol()->get_name();
    return dfg->findNode(name);
    Node* var_node = dfg->findNode(name);
    if (var_node == NULL) {
      var_node = new VarNode(name);
      dfg->addNode(var_node);
    }
    return var_node;
  }

  if (isSgValueExp(ex)!= NULL) {
    Node *n = new ConstantNode(isSgValueExp(ex)->get_constant_folded_value_as_string());
    dfg->addSource(n);
    return n;
  }

  if (isSgPntrArrRefExp(ex)) {
    SgPntrArrRefExp *ee = isSgPntrArrRefExp(ex);
    Node* left = toExprNodeRec(ee->get_lhs_operand());
    Node* offset_node = toNodePntrArrRef(isSgPntrArrRefExp(ex));
    if (left != NULL)
      left->addNeighbour(offset_node);
    return offset_node;
  }

  if (isSgBinaryOp(ex)) {
    SgBinaryOp *e = isSgBinaryOp(ex);
    Node *right = toExprNodeRec(e->get_rhs_operand());

    // assignments are a bit special since we need to add the
    // buffering before the assignment to the lhs stream
    if (isSgAssignOp(e) && isSgPntrArrRefExp(e->get_lhs_operand())) {
      // handles: y[offset_expression] = expression
      SgPntrArrRefExp *ee = isSgPntrArrRefExp(e->get_lhs_operand());
      string name = ee->get_lhs_operand()->unparseToString();
      Node* offset_node = toNodePntrArrRef(ee);

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
    }

    if (isSgAssignOp(e) && isSgVarRefExp(e->get_lhs_operand())) {
      //right->addNeighbour(left);
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

    Node *left = toExprNodeRec(e->get_lhs_operand());
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
    else if (isSgDivideOp(e))
      op = "/";

    //Node *left = toExprNodeRec(e->get_lhs_operand());
    Node *node = new OpNode(op);
    //check whether its root is a SgPntrArrRefExp, if so, not an arithmetic node
    dfg->addArith(node);
    dfg->addNode(node);
    if (right != NULL) {
      right->addNeighbour(node);
      node->addInput(right);
    }
    if (left != NULL) {
      left->addNeighbour(node);
      node->addInput(left);
    }
    //add to the arithmetics group
    return node;
  }

  if (isSgUnaryOp(ex)) {
    SgUnaryOp *unOp = isSgUnaryOp(ex);
    Node *n = toExprNodeRec(unOp->get_operand());
    if (n != NULL) {
      D(cout << "Node " << n;)
      string op;
      if (isSgMinusOp(ex))
        op = "-";
      OpNode *node = new OpNode(op);
      n->addNeighbour(node);
      node->addInput(n);
      dfg->addNode(node);
      //add to the arithmetics group
      dfg->addArith(node);
      return node;
    }
  }

  if (isSgConditionalExp(ex)) {
    SgConditionalExp* cex = isSgConditionalExp(ex);
    Node* cond = toExprNode(cex->get_conditional_exp());
    Node* ifTrue = toExprNode(cex->get_true_exp());
    Node* ifFalse = toExprNode(cex->get_false_exp());

    MuxNode* muxNode = new MuxNode();

    if (cond != NULL)
      cond->addNeighbour(muxNode);

    if (ifTrue != NULL)
      muxNode->setIfTrueNode(ifTrue);

    if (ifFalse != NULL)
      muxNode->setIfFalseNode(ifFalse);
    return muxNode;
  }

  D(cerr << "Couldn't create node for: " + ex->unparseToString() << endl);
  cout << "Couldn't create node for: " + ex->unparseToString() << endl;
  return NULL;
}

void ASTtoDFGVisitor :: atTraversalEnd() {}
