#include "ExtractStencil.h"
#include "../Design.hxx"
#include "../Stencil.h"
#include "../utils.hxx"
#include "../StencilOffset.h"

#include <boost/lexical_cast.hpp>

#include <queue>

NodeQuerySynthesizedAttributeType findStencils(SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  string stencilPragma = "#pragma fast stencil";

  SgPragmaDeclaration* pragma = isSgPragmaDeclaration(astNode);
  if (pragma != NULL) {
    string name = pragma->get_pragma()->unparseToString();
    // TODO infer stencil shape ...
    if (name.substr(0,stencilPragma.size()) == stencilPragma)
      returnNodeList.push_back(astNode);
  }

  return returnNodeList;
}

NodeQuerySynthesizedAttributeType findOffsetExpressions(SgNode *astNode) {
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  SgPntrArrRefExp* exp = isSgPntrArrRefExp(astNode);
  if (exp != NULL)
    returnNodeList.push_back(exp);

  return returnNodeList;
}

int ExtractStencil::getStencilDimension(SgStatement *st) {
  return getLoopVariables(st).size();
}

vector<string> ExtractStencil::getLoopVariables(SgStatement *st) {
  vector<string> vars;
  while (isSgForStatement(st)) {
    SgInitializedName* v = SageInterface::getLoopIndexVariable(st);
    vars.push_back(v->unparseToString());
    st = isSgBasicBlock(isSgForStatement(st)->get_loop_body());
    if (st == NULL)
      break;
    int st_count = isSgBasicBlock(st)->get_statements().size();
    if (st_count == 1 )
      st = isSgBasicBlock(st)->get_statements()[0];
  }

 return vars;
}

SgAssignOp* ExtractStencil::getStencilUpdateAssignment(SgStatement *st) {
  SgBasicBlock *bb = isSgBasicBlock(getStencilUpdateStatement(st));
  if (bb != NULL && !bb->get_statements().empty()) {
    // assume stencil update assignment is the first statement
    SgExprStatement* expr = isSgExprStatement(bb->get_statements()[0]);
    SgAssignOp *op;
      if (expr != NULL && 
	  (op = isSgAssignOp(expr->get_expression())) != NULL) {
      return op;
    }
  }

  return NULL;
}

SgStatement* ExtractStencil::getStencilUpdateStatement(SgStatement *st) {
  SgForStatement* for_stmt;
  while ( (for_stmt = isSgForStatement(st)) != NULL) {
    SgBasicBlock* bb = isSgBasicBlock(for_stmt->get_loop_body());

    if (bb == NULL)
      return st;

    st = bb;

    if (bb->get_statements().size() == 1) {
      SgForStatement* f = isSgForStatement(bb->get_statements()[0]);
      if (f)
	st = f;
    }
  }

  return st;
}

vector<string> ExtractStencil::getStencilOutputs(SgStatement *st) {
  SgStatement* update_st = getStencilUpdateStatement(st);
  set<SgInitializedName*> write_vars, read_vars;
  SageInterface::collectReadWriteVariables(update_st, read_vars, write_vars);
  vector<string> vars;
  foreach_(SgInitializedName* out, write_vars) {
    vars.push_back(out->unparseToString());
  }
  return vars;
}

vector<string> ExtractStencil::getStencilInputs(SgStatement *st) {
  SgStatement* update_st = getStencilUpdateStatement(st);
  set<SgInitializedName*> read_only_vars;
  SageInterface::collectReadOnlyVariables(update_st, read_only_vars);
  vector<string> vars;
  foreach_(SgInitializedName* in, read_only_vars) {
    vars.push_back(in->unparseToString());
  }
  return vars;
}

int ExtractStencil::getStencilShape(SgStatement *st) {
  vector<string> loop_vars = getLoopVariables(st);
  SgStatement* s = getStencilUpdateStatement(st);
  return 1;
}

pair<string, int> getStencilOffset(SgBinaryOp* binOp,
		     SgExpression *other, vector<string> loopVars, string var) {

  vector<string>::iterator it = find(loopVars.begin(), loopVars.end(), var);

  if (it != loopVars.end())  {
    cout << "Found potential offset " << *it << endl;
    // found a potential stencil offset
    int sign = isSgSubtractOp(binOp) ? -1 : 1;

    if (isSgSubtractOp(binOp) || isSgAddOp(binOp)) {
      SgValueExp *valExp;
      if ((valExp = isSgValueExp(other)) != NULL) {
	int dim = SageInterface::getIntegerConstantValue(valExp)  * sign;
	return make_pair(*it, dim);
      }
    }
    
    return make_pair(*it, 0);
  }

  return make_pair("", 0);
}


string ExtractStencil::getDimensionForOffset(SgExpression* expr, vector<string> loopVars) {
  SgBinaryOp* parent = isSgBinaryOp(expr->get_parent());
  if (parent == NULL)
    return "";
  SgVarRefExp* varRef = isSgVarRefExp(parent->get_lhs_operand());
  if (varRef == NULL)
    varRef = isSgVarRefExp(parent->get_rhs_operand());
  if (varRef == NULL)
    return "";
  return varRef->unparseToString();
}

vector<StencilOffset* > ExtractStencil::getOffsetsByDimension(vector<SgExpression* > offsets,
							      Stencil *stencil) {

  vector<string> loopVars = stencil->getLoopVariables();

  map<string, vector<int> > offset_map;

  vector<StencilOffset *> ret_offsets;

  foreach_(SgExpression *e, offsets) {

    queue<SgExpression*> q;
    q.push(e);

    StencilOffset* s = new StencilOffset();
    while (!q.empty()) {
      SgExpression* exp = q.front(); q.pop();
      SgBinaryOp* binOp = isSgBinaryOp(exp);

      if ( binOp != NULL ) {
	cout << binOp->unparseToString() << endl;
	SgVarRefExp* varRef;
	SgVarRefExp* varRef2;
	if (isSgMultiplyOp(binOp) &&
	    (varRef = isSgVarRefExp(binOp->get_lhs_operand())) != NULL &&
	    (varRef2 = isSgVarRefExp(binOp->get_rhs_operand())) != NULL ) {
	  string varname1 = varRef->unparseToString();
	  string varname2 = varRef2->unparseToString();
	  vector<string>::iterator it = find(loopVars.begin(), loopVars.end(), varname1);
	  if (it != loopVars.end()) {
	    s->var_offset[varname1] = s->dim_offset[varname2] = 0;
	  } else {
	    it = find(loopVars.begin(), loopVars.end(), varname2);
	    if (it != loopVars.end()) {
	      s->var_offset[varname2] = s->dim_offset[varname1] = 0;
	    }
	  }
	} else if ((varRef = isSgVarRefExp(binOp->get_lhs_operand())) != NULL) {
	  
	  pair<string, int> offset = getStencilOffset(binOp, binOp->get_rhs_operand(),
						      loopVars, varRef->unparseToString());
	  if (offset.first != "") {
	    s->var_offset[offset.first] = offset.second;
	    // found an offset look for the dimension
	    string dim = getDimensionForOffset(binOp, loopVars);
	    s->dim_offset[dim] = offset.second;
	  }
	  q.push(binOp->get_rhs_operand());
	} else if ((varRef = isSgVarRefExp(binOp->get_rhs_operand())) != NULL) {
	  pair<string, int> offset = getStencilOffset(binOp, binOp->get_lhs_operand(),
						      loopVars, varRef->unparseToString());
	  if (offset.first != "") {
	    s->var_offset[offset.first] = offset.second;
	    	    string dim = getDimensionForOffset(binOp, loopVars);
	    s->dim_offset[dim] = offset.second;
	  }
	  q.push(binOp->get_lhs_operand());
	} else {
	  q.push(binOp->get_lhs_operand());
	  q.push(binOp->get_rhs_operand());
	}
      }
    }

    ret_offsets.push_back(s);
  }

  cout << "Found offsets " << endl;
  foreach_(StencilOffset* of, ret_offsets) {
    map<string, int>::iterator it;
    cout << "vars: ";
    for (it = of->var_offset.begin(); it != of->var_offset.end(); it++) {
      cout << it->first << " " << it->second << ",";
    }
    cout << " dims: ";
    for (it = of->dim_offset.begin(); it != of->dim_offset.end(); it++) {
      cout << it->first << " " << it->second << ",";
    }
    cout << endl;
  }

  return ret_offsets;
}

vector<StencilOffset* > ExtractStencil::getStencilOffsets(SgStatement *st, Stencil *stencil) {
  vector<pair<string, string> > offsets;

  SgAssignOp* update_st = getStencilUpdateAssignment(st);

  Rose_STL_Container<SgNode*> offset_expressions;
  offset_expressions = NodeQuery::queryNodeList(
      NodeQuery::querySubTree(update_st, V_SgPntrArrRefExp),
      &findOffsetExpressions);


  map<string, vector<SgExpression* > > offset_map;

  cout << "Found offset expressions" << endl;
  foreach_ (SgNode* node, offset_expressions) {
    cout << node->unparseToString() << endl;
    SgPntrArrRefExp* pntr_ref = isSgPntrArrRefExp(node);
    SgExpression* expr = pntr_ref->get_lhs_operand();
    if (expr != NULL && isSgVarRefExp(expr)) {
      string var = isSgVarRefExp(expr)->unparseToString();
      offset_map[var].push_back(pntr_ref->get_rhs_operand());
    } else {
      // TODO handle error
    }
  }

  map<string, vector<SgExpression*> >::iterator it;
  for (it = offset_map.begin(); it != offset_map.end(); it++) {	
    cout << it->first << "\n\t";
    foreach_(SgExpression *e, it->second) {
      cout << e->unparseToString() << ", ";
    }
    cout << endl;
  }

  int max = 0;
  string max_vec;
  for (it = offset_map.begin(); it != offset_map.end(); it++) {	
    if (it->second.size() > max)
      max_vec = it->first;
  }

  
  // The convolution source is assumed to be the one with the most offset expressions
  string conv_src = max_vec;

  // The convolution destination is the lhs of the update assignment
  SgPntrArrRefExp* lhs = isSgPntrArrRefExp(update_st->get_lhs_operand());	
  string conv_dest;
  if (lhs != NULL && isSgVarRefExp(lhs->get_lhs_operand())) {
    conv_dest = lhs->get_lhs_operand()->unparseToString();
  } else {
    // TODO handle errors
  }

  // offsets by dimension from slow -> fast
  vector<SgExpression* > exprs = offset_map[max_vec];
  vector<StencilOffset* > offsetsByDimension = getOffsetsByDimension(exprs, stencil);

  stencil->setSource(conv_src);
  stencil->setDestination(conv_dest);

  return offsetsByDimension;
}

void ExtractStencil::runPass(Design* design) {
  SgProject* project = design->getProject();
  Rose_STL_Container<SgNode*> stencil_pragmas;
  stencil_pragmas = NodeQuery::queryNodeList(
      NodeQuery::querySubTree(project, V_SgPragmaDeclaration),
      &findStencils);

  foreach_ (SgNode* node, stencil_pragmas) {
    SgPragmaDeclaration *pragma = isSgPragmaDeclaration(node);
    if (pragma != NULL) {
      SgStatement* st = SageInterface::getNextStatement(pragma);

      int dim = getStencilDimension(st);
      int shape = getStencilShape(st);
      
      vector<string> inputs = getStencilInputs(st);
      vector<string> outputs = getStencilOutputs(st);
      vector<string> loopVars = getLoopVariables(st);

      cout << "Update assignment " << getStencilUpdateAssignment(st)->unparseToString() << endl;

      Stencil *s = new Stencil(dim, inputs, outputs);
      s->setLoopVariables(loopVars);

      vector<StencilOffset*> offsets = getStencilOffsets(st, s);
      s->setOffsets(offsets);
      design->addStencil(s);
    }
  }
}


string ExtractStencil::logPass() {
  return "Stencil Extraction";
}
