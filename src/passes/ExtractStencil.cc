#include "ExtractStencil.h"
#include "../Design.hxx"
#include "../Stencil.h"

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

SgStatement* ExtractStencil::getStencilUpdateAssignment(SgStatement *st) {
  SgBasicBlock *bb = isSgBasicBlock(getStencilUpdateStatement(st));
  if (bb != NULL && !bb->get_statements().empty()) {
    // assume stencil update assignment is the first statement
    SgExprStatement* expr = isSgExprStatement(bb->get_statements()[0]);
    if (expr != NULL && isSgAssignOp(expr->get_expression())) {
      return expr;
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

      design->addStencil(s);
    }
  }
}


string ExtractStencil::logPass() {
  return "Stencil Extraction";
}
