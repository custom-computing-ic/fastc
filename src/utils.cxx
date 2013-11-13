#include "utils.hxx"

#include <boost/filesystem.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <queue>

std::string* get_pragma_param(std::string pragma, std::string param) {
  using namespace std;
  int pos = pragma.find(param);
  string param_value = "";
  if (pos != string::npos) {
    int semic1 = pragma.find(":", pos);
    int semic2 = pragma.find(":", semic1 + 1);
    if (semic2 == string::npos) {
      param_value = pragma.substr(semic1 + 1);
    } else {
      int p2 = semic2 - 2;
      while (pragma[p2] != ' ') {
        p2--;
      }
      param_value = pragma.substr(semic1 + 1, p2 - semic1 - 1);
    }
  }
  boost::algorithm::trim(param_value);
  return new string(param_value);
}

SgProject* load_rose_project(int argc, char** argv) {
  SgProject* project = frontend(argc, argv);
  //  AstTests :: runAllTests(project);
  if (project == NULL) {
    std::cerr << "Could not run compiler frontend! Shutting down! " << std::endl;
    exit(0);
  }

  return project;
}


void setupBuild() {
  using namespace boost::filesystem;
  path dir("build"), cpu("build/cpu"), engine("build/engine");
  create_directory(dir);
  create_directory(cpu);
  create_directory(engine);
}


std::vector<std::string> getFunctionArgNames(SgFunctionDeclaration* fdecl) {
  std::vector<std::string> args;
  foreach_(SgInitializedName* arg, fdecl->get_args()) {
    args.push_back(arg->unparseToString());
  }
  return args;
}


std::vector<std::string> getFunctionCallArgNames(SgFunctionCallExp* exp) {
  std::vector<std::string> args;
  foreach_(SgExpression* arg, exp->get_args()->get_expressions()) {
    args.push_back(arg->unparseToString());
  }
  return args;

}

bool isVarOrConstant(SgExpression* exp) {
  return (isSgVarRefExp(exp) != NULL) ||
    (isSgValueExp(exp) != NULL);
}


StencilOffset* StencilUtils::extractSingleOffset(SgExpression* e, Stencil* stencil) {
  using namespace std;

  queue<SgExpression*> q;
  q.push(e);
  vector<string> loopVars = stencil->getLoopVariables();

  StencilOffset* s = new StencilOffset(stencil);
  while (!q.empty()) {
    SgExpression* exp = q.front(); q.pop();
    SgBinaryOp* binOp = isSgBinaryOp(exp);

    if ( binOp != NULL ) {
      SgExpression* varRef;
      SgExpression* varRef2;
      if (isSgMultiplyOp(binOp) &&
          isVarOrConstant(binOp->get_lhs_operand()) &&
          isVarOrConstant(binOp->get_rhs_operand())
          /*	  (varRef = isSgVarRefExp(binOp->get_lhs_operand())) != NULL &&
                (varRef2 = isSgVarRefExp(binOp->get_rhs_operand())) != NULL */) {
        varRef = binOp->get_lhs_operand();
        varRef2 = binOp->get_rhs_operand();
        string varname1 = varRef->unparseToString();
        string varname2 = varRef2->unparseToString();
        vector<string>::iterator it = find(loopVars.begin(), loopVars.end(), varname1);
        if (it != loopVars.end()) {
          s->var_offset[varname1] = s->dim_offset[varname2] = 0;
          s->var_dim[varname1] = varname2;
        } else {
          it = find(loopVars.begin(), loopVars.end(), varname2);
          if (it != loopVars.end()) {
            s->var_offset[varname2] = s->dim_offset[varname1] = 0;
            s->var_dim[varname2] = varname1;
          }
        }
      } else if ((varRef = isSgVarRefExp(binOp->get_lhs_operand())) != NULL) {
        pair<string, int> offset = getStencilOffset(binOp, binOp->get_rhs_operand(),
            loopVars, varRef->unparseToString());
        if (offset.first != "") {
          s->var_offset[offset.first] = offset.second;
          // found an offset look for the dimension
          string dim = getDimensionForOffset(binOp);
          s->dim_offset[dim] = offset.second;
          s->var_dim[offset.first] = dim;
        }
        q.push(binOp->get_rhs_operand());
      } else if ((varRef = isSgVarRefExp(binOp->get_rhs_operand())) != NULL) {
        pair<string, int> offset = getStencilOffset(binOp, binOp->get_lhs_operand(),
            loopVars, varRef->unparseToString());
        if (offset.first != "") {
          s->var_offset[offset.first] = offset.second;
          string dim = getDimensionForOffset(binOp);
          s->dim_offset[dim] = offset.second;
          s->var_dim[offset.first] = dim;
        }
        q.push(binOp->get_lhs_operand());
      } else {
        q.push(binOp->get_lhs_operand());
        q.push(binOp->get_rhs_operand());
      }
    }
  }

  return s;
}

std::string StencilUtils::getDimensionForOffset(SgExpression* expr) {
  using namespace std;
  SgBinaryOp* parent = isSgBinaryOp(expr->get_parent());
  if (parent == NULL)
    return "";
  SgExpression* varRef = isSgVarRefExp(parent->get_lhs_operand());
  if (varRef == NULL)
    varRef = isSgVarRefExp(parent->get_rhs_operand());

  if (varRef == NULL)
    varRef = isSgValueExp(parent->get_lhs_operand());

  if (varRef == NULL)
    varRef = isSgValueExp(parent->get_rhs_operand());

  if (varRef == NULL)
    return "";

  return varRef->unparseToString();
}


std::pair<std::string, int> StencilUtils::getStencilOffset(SgBinaryOp* binOp,
    SgExpression *other,
    std::vector<std::string> loopVars,
    std::string var) {
  using namespace std;
  vector<string>::iterator it = find(loopVars.begin(), loopVars.end(), var);

  if (it != loopVars.end())  {
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
