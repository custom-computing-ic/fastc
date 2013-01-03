#include "AstToDfgVisitor.hxx"

#include "DataFlowGraph/Node.hxx"
#include "DotDFSVisitor.hxx"
#include "MaxJVisitor.hxx"

using namespace std;
using namespace boost;


ASTtoDFGVisitor :: ASTtoDFGVisitor() {
  PRAGMA_IN     = new regex("in (\\w*) (\\w*) (\\w*)");
  PRAGMA_OUT    = new regex("out (\\w*) (\\w*) (\\w*)");
  PRAGMA_SCALAR_IN = new regex("scalar in (\\w*)");
  KERNEL_FUNC = new regex("kernel_.*");
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
      //      cout << "Added simple counter: " << *counterNode << endl;
      dfg.addSource(counterNode);
    } else if (fname.compare("countChain") == 0) {

      string p = "";
      SgVarRefExp *parent = isSgVarRefExp(*(++itt));
      if (parent != NULL)
	p = parent->get_symbol()->get_name();

      CounterNode *counterNode = new CounterNode(variableName, d, i, p);
	  
      Node *childNode = dfg.findNode(p);
      //      cout << "Added chained counter: " << *counterNode << endl;
      if ( childNode != NULL ) {
	counterNode->addNeighbour(childNode);
	dfg.removeSource(childNode);
      }

      dfg.addSource(counterNode);
    } 
  } else if (fname.compare("fselect") == 0) {

    MUXNode *muxNode = new MUXNode(variableName);
    dfg.addNode(muxNode);

    SgExpression *condExp = isSgExpression(*itt);
    Node *exp = toExprNode(*itt);
    //dfg.addSource(exp);
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
    dfg.addSource(n);
    return n;
  } 

  SgVarRefExp *var = isSgVarRefExp(ex);
  if (var != NULL)
    return dfg.findNode(var->get_symbol()->get_name());

  return NULL;
}

Node* ASTtoDFGVisitor :: toExprNode(SgExpression *ex) {
  return toExprNodeRec(ex);
}

Node* ASTtoDFGVisitor :: toExprNodeRec(SgExpression *ex) {
  if ( isSgVarRefExp(ex) ) {
    SgVarRefExp *e = isSgVarRefExp(ex);
    string name = e->get_symbol()->get_name();
    return dfg.findNode(name);
  } else if (isSgIntVal(ex) )  {
    SgIntVal *e = isSgIntVal(ex);
    stringstream out;
    out << e->get_value();
    string val = out.str();
    Node *n = dfg.findNode(val);
    if (n == NULL) {
      n = new ConstantNode(val);
      dfg.addSource(n);
    }
    return n;
  } else if (isSgBinaryOp(ex)) {

    SgBinaryOp *e = isSgBinaryOp(ex);
    Node *left = toExprNodeRec(e->get_lhs_operand());
    Node *right = toExprNodeRec(e->get_rhs_operand());

    string op;
    if (isSgAddOp(ex))
      op = "+";
    else if (isSgMultiplyOp(ex))
      op = "*";
    else if (isSgAndOp(ex) || isSgBitAndOp(ex) )
      op = "&";
    else if (isSgLessThanOp(ex))
      op = "<";
    else if (isSgGreaterThanOp(ex))
      op = ">";

    Node *node;
    if ( isSgPntrArrRefExp(ex))
      node = new StreamOffsetNode("Offset");
    else
      node = new OpNode(op);

    dfg.addNode(node);
    if (right != NULL)
      right->addNeighbour(node);
    if (left != NULL)
      left->addNeighbour(node);

    return node;

  } else if (isSgUnaryOp(ex)) {
    SgUnaryOp *unOp = isSgUnaryOp(ex);
    Node *n = toExprNodeRec(unOp->get_operand());
    string op;
    if (isSgMinusOp(ex))
      op = "-";
    OpNode *node = new OpNode(op);
    n->addNeighbour(node);
    dfg.addNode(node);
    return node;
  }

  return NULL;
}  



void ASTtoDFGVisitor :: visit(SgNode *n) {

  if (isSgVariableDeclaration(n)) {
    SgVariableDeclaration *varDecl = isSgVariableDeclaration(n);

    SgInitializedNamePtrList vars = varDecl->get_variables();
    SgInitializedNamePtrList::iterator it;

    for (it = vars.begin(); it != vars.end(); it++) {
      SgInitializedName *v = *it;
      string variableName = (*it)->get_qualified_name();

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
	dfg.addNode(n);
	dfg.addNode(varNode);
	n->addNeighbour(varNode);
      }  


      
    }

  } else if (isSgPragma(n))  {
    SgPragma* pragma = (SgPragma *)n;
    string s = pragma->get_pragma();
    cmatch sm;
    if ( regex_match(s.c_str(), sm, *PRAGMA_IN) ) {
      InputNode *inputNode = new InputNode(sm[1]);
      inputs.push_front(*inputNode);
      dfg.addInputNode(inputNode);
      //      cout << "Added input node: " << *inputNode << endl;
    } else if (regex_match(s.c_str(), sm, *PRAGMA_OUT)) {
      OutputNode *outputNode = new OutputNode(sm[1]);
      outputs.push_front(*outputNode);
      dfg.addOutputNode(outputNode);
      //      cout << "Added output node: " << *outputNode << endl;
    } else if (regex_match(s.c_str(), sm, *PRAGMA_SCALAR_IN)) {
      InputNode *inputNode = new InputNode(sm[1]);
      inputs.push_front(*inputNode);
      dfg.addInputNode(inputNode);
      //      cout << "Added scalar input node: " << *inputNode << endl;
    }
  } else if (isSgFunctionCallExp(n)) {
    SgFunctionCallExp *fcall = isSgFunctionCallExp(n);
    SgExpressionPtrList args = fcall->get_args()->get_expressions();
    SgExpressionPtrList::iterator it;
    it=args.begin();
    SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
    string fname = fsymbol->get_name();
      // output()
    if (fname.compare("output") == 0) {
      Node *outputNode = toNode(*it);
      //      cout << "Output Node is" << *outputNode <<endl;
      Node *result     = toNode(*(++it));
	if (result != NULL)
	  result->addNeighbour(outputNode);
    }
  }
   
}

void ASTtoDFGVisitor :: atTraversalEnd() {
  DotDFSVisitor dfsVisitor(&dfg);
  dfsVisitor.traverse();
  //  cout << dfsVisitor.getDot();

  MaxJVisitor maxJVisitor(&dfg);
  maxJVisitor.traverse();
  cout << maxJVisitor.getMaxJCode();
}
