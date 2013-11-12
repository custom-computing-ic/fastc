#include "precompiled.hxx"
#include "utils.hxx"
#include "StencilAstToDfgVisitor.h"

#include "DataFlowGraph/Node.hxx"
#include "DataFlowGraph/MuxNode.hxx"
#include "DataFlowGraph/CounterNode.hxx"

using namespace std;
using namespace boost;

StencilAstToDfgVisitor :: StencilAstToDfgVisitor(Kernel *k) {
  this->kernel = k;
  this->dfg = k->getDataFlowGraph();
}

void StencilAstToDfgVisitor :: visit(SgNode *n) {

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


void StencilAstToDfgVisitor :: function_call_initializer(string& variableName, SgFunctionCallExp *fcall) {
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

Node* StencilAstToDfgVisitor :: toNode(SgExpression *ex) {

  SgValueExp *valExp = isSgValueExp(ex);

  if (valExp != NULL) {
    Node *n = new ConstantNode(valExp->get_constant_folded_value_as_string());
    dfg->addSource(n);
    return n;
  }

  SgVarRefExp *var = isSgVarRefExp(ex);
  if (var != NULL)
    return dfg->findNode(var->get_symbol()->get_name());

  return NULL;
}

Node* StencilAstToDfgVisitor :: toExprNode(SgExpression *ex) {
  return toExprNodeRec(ex);
}

Node* StencilAstToDfgVisitor :: toExprNodeRec(SgExpression *ex) {
  if (isSgVarRefExp(ex) ) {
    string name = isSgVarRefExp(ex)->get_symbol()->get_name();
    return dfg->findNode(name);
  } else if (isSgValueExp(ex)!= NULL) {
    Node *n = new ConstantNode(isSgValueExp(ex)->get_constant_folded_value_as_string());
    dfg->addSource(n);
    return n;
    /*
    SgIntVal *e = isSgIntVal(ex);
    string val = e->unparseToString();
    Node *n = dfg->findNode(val);
    if (n == NULL) {
      n = new ConstantNode(val);
      dfg->addSource(n);
    }
    return n; */
  } else if (isSgBinaryOp(ex)) {

    SgBinaryOp *e = isSgBinaryOp(ex);

    Node *right = toExprNodeRec(e->get_rhs_operand());

    if (isSgAssignOp(e)) {

      // assignments are a bit special
      if (isSgPntrArrRefExp(e->get_lhs_operand())) {
        // handles: y[offset_expression] = expression
        StreamOffsetNode* offset_node = new StreamOffsetNode("Offset");

        SgPntrArrRefExp *ee = isSgPntrArrRefExp(e->get_lhs_operand());
        string name = ee->get_lhs_operand()->unparseToString();


	// extract stencil offsets from this expression
	StencilOffset* stencilOffset =
	  StencilUtils::extractSingleOffset(ee->get_rhs_operand(),
					    kernel->getFirstStencil());
	
		
        //search if the current input has been bufferred
        Offset* offset = dfg->findOffset(name);
	//	
        if (offset==NULL)//no such buffer
        {
          offset = new Offset(name);
          offset->offsets.push_back(ee->get_rhs_operand()->unparseToString());
          dfg->addOffset(offset); 
        }
        else 
          offset->offsets.push_back(ee->get_rhs_operand()->unparseToString());
	
	offset->setStencilOffset(stencilOffset);
	offset_node->setStencilOffset(stencilOffset);
	
        cout<<"buffer "<<offset->getName()<<endl;
        for (list<string>::iterator it = offset->offsets.begin(); it!=offset->offsets.end(); ++it)
           cout<<" "<< *it;
        cout<<endl;

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


    string op("");
    
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
    {
      node = new StreamOffsetNode("Offset");

      // extract stencil offsets from this expression
      StencilOffset* stencilOffset =
	StencilUtils::extractSingleOffset(ex, kernel->getFirstStencil());
      ((StreamOffsetNode* )node)->setStencilOffset(stencilOffset);
      
      //serach if the current input has been bufferred
      SgPntrArrRefExp *ee = isSgPntrArrRefExp(ex);
      string name = ee->get_lhs_operand()->unparseToString();
      Offset* offset = dfg->findOffset(name);
      if(offset==NULL)//no such buffer
      {
        offset = new Offset(name);
        offset->offsets.push_back(ee->get_rhs_operand()->unparseToString());
        dfg->addOffset(offset); 
      }
      else 
        offset->offsets.push_back(ee->get_rhs_operand()->unparseToString());
      cout<<"buffer "<<offset->getName()<<endl;
      for(list<string>::iterator it = offset->offsets.begin(); it!=offset->offsets.end(); ++it)
        cout<<" "<< *it;
      cout<<endl;
    } else if (op == "" ) {
      return right;
    } else {
      node = new OpNode(op);
      dfg->addArith(node);
    }
    dfg->addNode(node);
    if (right != NULL)
    {
      right->addNeighbour(node);
      node->addInput(right);
    }
    if (left != NULL)
    {
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
      cout << "Node " << n;
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

void StencilAstToDfgVisitor :: atTraversalEnd() {}
