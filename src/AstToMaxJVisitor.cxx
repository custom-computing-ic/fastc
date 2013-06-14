#include "precompiled.hxx"
#include "AstToMaxJVisitor.hxx"
#include "utils.hxx"

using namespace std;
using namespace boost;

class SgFunctionSymbol;
class SgVarRefExp;
class SgIntVal;
class SgBinaryOp;
class SgUnaryOp;

ASTtoMaxJVisitor::ASTtoMaxJVisitor() {
  this->kernel = NULL;
  paramCount = 0;
  source = "";
}

ASTtoMaxJVisitor::ASTtoMaxJVisitor(Kernel* kernel) {
  this->kernel = kernel;
  paramCount = 0;
  source = "";
}

void ASTtoMaxJVisitor::preOrderVisit(SgNode *n) {
  if ( n == NULL || (n != NULL && n->getAttribute("ignore") != NULL) )
    return;

  D(cerr << "Visiting node: " << n->unparseToString() << endl);
  D(cerr << ",ignore: " << n->getAttribute("ignore") << endl);
  D(cerr << "get_name: " << SageInterface::get_name(n) << endl);

  if (isSgVariableDeclaration(n)) {
    visitVarDecl(isSgVariableDeclaration(n));
    ignoreChildren(n);
  }  else if (isSgFunctionCallExp(n)) {
    visitFcall(isSgFunctionCallExp(n));
  } else if (isSgForStatement(n)) {
    visitFor(isSgForStatement(n));
  } else if (isSgExprStatement(n)) {
    visitExprStmt(isSgExprStatement(n));
    ignoreChildren(n);
  } else if (isSgPragma(n)) {
    source += *visitPragma(isSgPragma(n));
  } else if (isSgExpression(n)) {
    string *e = toExpr(isSgExpression(n));
    if (e != NULL)
      source += *e;
  }
}

void ASTtoMaxJVisitor::ignoreChildren(SgNode *n) {
  vector<SgNode *> children = n->get_traversalSuccessorContainer();
  ignore(n);
  foreach_(SgNode *n, children) {
    if (n != NULL)
      ignoreChildren(n);
  }
}

string* ASTtoMaxJVisitor::visitPragma(SgPragma *n) {
  string *s = new string("pragma found");
  D(cerr << "Visiting pragma! " << endl);

  string *cls  = get_pragma_param(n->get_pragma(), "class");
  string *name = get_pragma_param(n->get_pragma(), "name");

  if (cls->compare("kernelopt") == 0) {
    if (name->compare("pushDSP") == 0) {
      string* factor = get_pragma_param(n->get_pragma(), "factor");
      return new string("optimization.pushDSPFactor("+*factor+");\n");
    } else if (name->compare("popDSP") == 0) {
      return new string("optimization.popDSPFactor();\n");
    }
  }

  return s;
}

void ASTtoMaxJVisitor::postOrderVisit(SgNode *n) {
  if (n != NULL  && isSgForStatement(n)) {
    source += "}\n";
  }
}

string* ASTtoMaxJVisitor::function_call_initializer(string& variableName,
                                                    SgFunctionCallExp *fcall) {
  string *s = new string();

  SgExpressionPtrList args = fcall->get_args()->get_expressions();
  SgExpressionPtrList::iterator itt = args.begin();
  SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
  string fname = fsymbol->get_name();

  if (fname == "count") {
    string *wrap = toExpr(*itt);

    SgIntVal *inc = isSgIntVal(*(++itt));
    string i;
    if (inc != NULL) {
      stringstream s;
      s << inc->get_value();
      i = s.str();
    }
    D(cerr << "Visiting count " << endl);
    // XXX it seems counter chains with one counter are supported?
    // TODO need a fresh variable for chain
    string name = "chain_" + variableName;
    declarations += "CounterChain " + name
      + " = control.count.makeCounterChain();\n";
    *s += "HWVar " + variableName + " = " + name + ".addCounter("
      + *wrap + ", " + i + ");\n";
    counterMap[variableName] = name;
  } else if (fname == "count_chain" ) {
    string *wrap = toExpr(*itt);
    SgIntVal *inc = isSgIntVal(*(++itt));
    string i;
    if (inc != NULL) {
      stringstream s;
      s << inc->get_value();
      i = s.str();
    }
    string p = "";
    SgVarRefExp *parent = isSgVarRefExp(*(++itt));
    if (parent != NULL)
      p = parent->get_symbol()->get_name();
    string chainDeclaration = counterMap[p];
    *s += "HWVar " + variableName + " = " + chainDeclaration
      + ".addCounter(" + *wrap + ", " + i + ");\n";
    counterMap[variableName] = chainDeclaration;
  } else if (fname == "count_p") {
    string param = "param" + lexical_cast<string>(paramCount);
    string *width = toExpr(*(itt));
    string *max   = toExpr(*(++itt));
    string *inc   = toExpr(*(++itt));
    string *enable = toExpr(*(++itt));
    *s += "Count.Params " + param + " = control.count.makeParams(" + *width + ")"
      + ".withMax(" + *max + ")"
      + ".withInc(" + *inc + ")";
    if (enable != NULL)
      *s += ".withEnable(" + *enable + ")";
    *s += ";\n";
    string counter = "counter" + lexical_cast<string>(paramCount);
    *s += "Counter " + counter + " = control.count.makeCounter(" + param + ");\n";
    *s += "HWVar " + variableName + " = " + counter + ".getCount();\n";
    paramCount++;
  } else if (fname == "fselect" || fname == "fselect_sf_f" ) {
    string *exp = toExpr(*itt);
    string *ifTrue = toExpr(*(++itt));
    string *ifFalse = toExpr(*(++itt));
    *s += "HWVar " + variableName + " = control.mux(" + (*exp) + ", "
      + (*ifTrue) + ", " + (*ifFalse) + ");\n";
  } else if (fname == "make_offset") {
    string *min  = toExpr(*itt);
    string *max  = toExpr(*(++itt));
    *s += "OffsetExpr " + variableName;
    *s += " = stream.makeOffsetParam(\""+ variableName + "\"," + *min + "," + *max + ");\n";
  } else if (fname == "make_array_f") {
    string *mantissa = toExpr(*itt);
    string *exponent = toExpr(*++itt);
    string *width    = toExpr(*++itt);
    string type = "hwFloat("+*mantissa+","+*exponent+")";
    *s += "KArray<HWVar>" + variableName;
    *s += " = (new KArrayType<HWVar>("+type+",";
    *s += *width+")).newInstance(this);\n";
  } else if (fname == "make_input_array_f") {
    string *mantissa = toExpr(*itt);
    string *exponent = toExpr(*++itt);
    string *width    = toExpr(*++itt);
    string type = "hwFloat("+*mantissa+","+*exponent+")";
    string t ="new KArrayType<HWVar>(" + type + "," + *width + ")";
    *s += "KArray<HWVar>" + variableName;
    *s += " = io.input(\"" + variableName + "\",  " + t +")";
  } else if (fname == "sqrt_i") {
    string* range_min = toExpr(*itt);
    string* range_max = toExpr(*(++itt));
    string* base      = toExpr(*(++itt));
    string* width     = toExpr(*(++itt));
    int num = 0; // XXX should be a unique identifier
    string range="range"+lexical_cast<string>(num);
    *s+= "KernelMath.Range "+range+ " = ";
    *s+= "new KernelMath.Range("+*range_min+", "+*range_max+");\n";
    string type = "hwUInt(" +*width+")";
    *s+= "HWVar "+variableName+" = KernelMath.sqrt("+range+", "+*base+","+type+");\n";
  } else if (fname == "cast_fix_i") {
    string *in = toExpr(*itt);
    string *exp = toExpr(*(++itt));
    string *mant = toExpr(*(++itt));
    *s += str(format("HWVar %s = %s.cast(hwFix(%s, %s, HWFix.SignMode.TWOSCOMPLEMENT));\n")
              % variableName % *in % *exp % *mant);
  }

  if ( s->size() == 0 ) {
    LOG_CERR();
    cerr << "Function definition not found! '" + fname + "'" << endl;
  }

  return s;
}

string* ASTtoMaxJVisitor::toExpr(SgExpression *ex) {
  if (isSgFunctionCallExp(ex)) {
    return visitFcall(isSgFunctionCallExp(ex));
  } if (isSgVarRefExp(ex)) {
    SgVarRefExp *e = isSgVarRefExp(ex);
    return new string(e->get_symbol()->get_name());
  } else if (isSgIntVal(ex) || isSgUnsignedLongVal(ex)) {
    SgValueExp *e = isSgValueExp(ex);
    return new string(e->get_constant_folded_value_as_string());
  } else if (isSgFloatVal(ex))  {
    SgFloatVal *e = isSgFloatVal(ex);
    return new string(e->get_valueString());
  } else if ( isSgDoubleVal(ex)) {
    SgDoubleVal *e = isSgDoubleVal(ex);
    return new string(e->get_valueString());
  } else if (isSgStringVal(ex)) {
    SgStringVal *e = isSgStringVal(ex);
    stringstream out;
    out << e->get_value();
    return new string("\"" + out.str() + "\"");
  } else if (isSgConditionalExp(ex)){
    SgConditionalExp* cex = isSgConditionalExp(ex);
    string* cond = toExpr(cex->get_conditional_exp());
    string* ifTrue = toExpr(cex->get_true_exp());
    string* ifFalse = toExpr(cex->get_false_exp());
    return new string(*cond+"?"+*ifTrue+":"+*ifFalse);
  } else if (isSgBinaryOp(ex)) {

    SgBinaryOp* e = isSgBinaryOp(ex);

    SgExpression* lhs = e->get_lhs_operand();
    SgExpression* rhs = e->get_rhs_operand();

    // a = f()
    if (isSgFunctionCallExp(rhs) && isSgVarRefExp(lhs)) {
      SgVarRefExp *e = isSgVarRefExp(lhs);
      string name = e->unparseToString();
      return function_call_initializer(name, isSgFunctionCallExp(rhs));
    }

    string op;
    string *right = toExpr(e->get_rhs_operand());
    string *left = toExpr(e->get_lhs_operand());

    if (isSgAssignOp(ex)) {
      op = "=";
      SgAssignOp *ass = isSgAssignOp(ex);
      // For stream arrays use <== instead of =
      SgExpression* nameExp;
      if (SageInterface::isArrayReference(ass->get_lhs_operand(), &nameExp)) {
        cout << "Array Reference " << nameExp->get_type()->unparseToString() << endl;
        // XXX: handle assigning constants
        if (kernel != NULL && kernel->isStreamArrayType(nameExp->unparseToString()))
          op = "<==";
      }
    }

    if (isSgAddOp(ex))
      op = "+";
    else if (isSgSubtractOp(ex))
      op = "-";
    else if (isSgMultiplyOp(ex))
      op = "*";
    else if (isSgAndOp(ex) || isSgBitAndOp(ex))
      op = "&";
    else if (isSgLessThanOp(ex))
      op = "<";
    else if (isSgGreaterThanOp(ex))
      op = ">";
    else if (isSgGreaterOrEqualOp(ex))
      op = ">=";
    else if (isSgLessOrEqualOp(ex))
      op = "<=";
    else if (isSgDivideOp(ex))
      op = "/";

    if (isSgEqualityOp(ex))
      return new string((*left) + ".eq(" + (*right) + ")");
    else if (isSgNotEqualOp(ex))
      return new string((*left) + ".neq(" + (*right) + ")");


    if (isSgPntrArrRefExp(ex)) {
      SgPntrArrRefExp *e = isSgPntrArrRefExp(ex);
      // check for the type of lhs
      string type =  e->get_type()->unparseToString();
      D(cerr << "PTR REF: Found ptr ref type " << type);
      D(cerr << "Expr: " << ex->unparseToString() << endl);
      D(cerr << "PTR REF: Type of lhs: ");
      D(cerr << e->get_lhs_operand()->get_type()->unparseToString() << endl);
      string lhs_type = e->get_lhs_operand()->get_type()->unparseToString();
      if (lhs_type.compare("float *") == 0 ||
          lhs_type.compare("int *") == 0) {
        // LHS is stream type so pointer access maps to stream.offset()

        // XXX optimisation should be done in a separate pass
        if ((*right).compare("0") == 0)
          return left;

        if (isConstant(*right)) {
          // constant offset value
          string* offset = new string();
          *offset += "stream.offset("+*left+", ";
          *offset += e->get_rhs_operand()->unparseToString() + ")";
          return offset;
        } else {
          // offset expression / dynamic offset
          // XXX limits for dynamic offsets should be inferred
          // first argument to offset() is HWVar
          // XXX for now, don't attempt to guess offset
          // bounds; Users should use the prev / next / ;
          // functions if they want to set limits
          return new  string("stream.offset("+*left+", "+*right+")");
          //+ ", -1024, 1024)");
        }
      }

      SgArrayType *t  = isSgArrayType(e->get_type());
      if (t != NULL) {
        vector<SgExpression*> expressions = SageInterface::get_C_array_dimensions(t);
        vector<SgExpression*>::iterator it;
        int dim = expressions.size() - 1;
        D(cerr << "Array type dim: " << dim << endl);

        if (dim >= 1) {
          return new string(ex->unparseToString());
        }
      } else {
        return new string(ex->unparseToString());
      }

      D(cerr << "PTR REF: No type found for exp: " << e->unparseToString());

    } else {
      if (isSgAssignOp(ex))
        // can't bracket assign stmts
        return new string((*left) + " " + op + " " + (*right));
      else
        return new string("(" + (*left) + " " + op + " " + (*right) + ")");
    }

  } else if (isSgUnaryOp(ex)) {
    SgUnaryOp *unOp = isSgUnaryOp(ex);
    string *exp = toExpr(unOp->get_operand());
    string op;
    if (isSgMinusOp(ex))
      op = "-";
    else if (isSgPlusPlusOp(ex))
      op = "++";
    else if (isSgPointerDerefExp(ex))
      op = "";

    if (exp != NULL)
      return new string(op + *exp);
  }

  return NULL;
}

void ASTtoMaxJVisitor::visitVarDecl(SgVariableDeclaration* decl) {

  SgVariableDeclaration *varDecl = isSgVariableDeclaration(decl);

  SgInitializedNamePtrList vars = varDecl->get_variables();

  cout << "IN HERE " << endl;
  SgInitializedNamePtrList::iterator it;
  for (it = vars.begin(); it != vars.end(); it++) {
    SgInitializedName *v = *it;
    string variableName = v->get_qualified_name();

    cout << "Variable decl : "  << variableName << endl;

    if (isSgArrayType(v->get_type()) ) {
      SgArrayType *t  = isSgArrayType(v->get_type());

      vector<SgExpression*> expressions = SageInterface::get_C_array_dimensions(t);
      vector<SgExpression*>::iterator it = expressions.begin();
      int dim = expressions.size() - 1;

      source += "HWVar " + variableName;
      for (int i = 0 ; i < dim; i++ ) {
        source += "[]";
      }
      source += " = new HWVar ";
      for (; it != expressions.end(); it++) {
        SgExpression* d = *(it);
        string *value = toExpr(d);
        if ( value != NULL )
          source += "[" + *value + "]";
      }
      source += ";\n";
      continue;
    }

    SgInitializer *init = v->get_initializer();
    SgAssignInitializer *ass = isSgAssignInitializer(init);
    if (ass == NULL) {
      // just a stream declaration, no assignment
      source += "HWVar " + variableName + ";\n";
      continue;
    }

    SgExpression *exp = ass->get_operand();

    // i = f() ..
    SgFunctionCallExp *fcall = isSgFunctionCallExp(exp);
    if (fcall != NULL) {
      source += *function_call_initializer(variableName, fcall);
      continue;
    }

    // i = expr;
    string* n = toExpr(exp);

    if (n != NULL) {
      if ( get_type(v)->compare("float") == 0 ||
           get_type(v)->compare("int") == 0) {
        // proper float or int constants
        source += *get_type(v) + " " + variableName;
        source += " = " + (*n)  + ";\n";
      } else {
        // stream type consts or varsb
        if (isConstant(*n))
          *n =  constVar(*n);
        source += "HWVar " + variableName + " = " + (*n) + ";\n";
      }
    }


  }
}

string* ASTtoMaxJVisitor::get_type(SgInitializedName *ident) {
  SgType *type = ident->get_type();
  return new string(type->unparseToString());
}

string* ASTtoMaxJVisitor::visitFcall(SgFunctionCallExp *fcall) {
  D(cerr << "Handling fcall " << endl);
  string type("hwFloat(8, 24)");
  SgExpressionPtrList args = fcall->get_args()->get_expressions();
  SgExpressionPtrList::iterator it = args.begin();
  SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
  string fname = fsymbol->get_name();

  string *s = new string();

  if (fname.compare("output_ic") == 0) {
    string* name = toExpr(*it);
    string* expr = toExpr(*(++it));
    string* cond = toExpr(*(++it));
    if (name == NULL || expr == NULL) {
      cerr << "NULL NAME!";
    } else if (cond == NULL)
      *s += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
        + ")";
    else {
      *s += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
        + ", " + (*cond) + ")";
    }
  } else if (fname.compare("output_f") == 0) {
    string* name = toExpr(*it);
    string* expr = toExpr(*(++it));
    if (name == NULL || expr == NULL) {
      cerr << "NULL NAME!";
    } else {
      *s += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
        + ")";
    }
  } else if (fname.compare("output_i") == 0) {
    string* name = toExpr(*it);
    string* expr = toExpr(*(++it));
    type = "hwInt(32)";
    if (name == NULL || expr == NULL) {
      cerr << "NULL NAME!";
    } else {
      *s += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
        + ")";
    }
  } else if (fname.compare("output_iaf") == 0) {
    string* name = toExpr(*it);
    string* expr = toExpr(*(++it));
    string* mantissa = toExpr(*(++it));
    string* exponent = toExpr(*(++it));
    string* width = toExpr(*(++it));
    string type = "new KArrayType<HWVar>(hwFloat("+*mantissa+", "+*exponent+"), "+*width+")";
    *s += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
      + ")";
  }  else if (fname.compare("DRAMOutput") == 0) {
    string *streamName = toExpr(*(it));
    string *control    = toExpr(*(++it));
    string *address    = toExpr(*(++it));
    string *size       = toExpr(*(++it));
    string *inc        = toExpr(*(++it));
    string *streamNo   = toExpr(*(++it));
    string *interrupt  = toExpr(*(++it));

    if (interrupt->compare("0") == 0)
      *interrupt = constVar("false");
    else if ( isConstant(*interrupt) )
      *interrupt = constVar("true");

    *s += "DRAMCommandStream.makeKernelOutput("
      + *streamName + ",\n"
      + *control + ",\n"
      + *address + ",\n"
      + constVar(*size, "hwUInt(8)") + ",\n"
      + constVar(*inc, "hwUInt(6)") + ",\n"
      + constVar(*streamNo, "hwUInt(4)") + ",\n"
      + *interrupt
      + ")";

  } else if (fname == "pushDSPFactor") {
    string *exp = toExpr(*it);
    *s += "optimization.pushDSPFactor(" + *exp + ")";
  } else if (fname == "popDSPFactor" ) {
    *s += "optimization.popDSPFactor()";
  } else if (fname == "fselect" || fname == "fselect_sf_f" ) {
    string *exp = toExpr(*it);
    string *ifTrue = toExpr(*(++it));
    string *ifFalse = toExpr(*(++it));
    *s += "control.mux("+*exp+", "+*ifTrue+", "+*ifFalse+")";
  } else if (fname == "cast2ff" || fname == "cast2sff" || fname == "cast2fsf") {
    string *out = toExpr(*it);
    string *in  = toExpr(*(++it));
    string *exponent = toExpr(*(++it));
    string *mantissa = toExpr(*(++it));
    string type = "hwFloat("+*exponent+", "+*mantissa+")";
    *s += *out+" = "+*in+".cast("+type+")";
  } else if (fname == "castf_f" || fname == "castf_sf") {
    string *in  = toExpr(*it);
    string *exponent = toExpr(*(++it));
    string *mantissa = toExpr(*(++it));
    string type = "hwFloat("+*exponent+", "+*mantissa+")";
    *s += *in+".cast("+type+")";
  } else if (fname == "exp") {
    string* power = toExpr(*it);
    *s += str(format("KernelMath.exp(%s)") % (*power));
  } else if (fname == "pushRoundingMode") {
    *s+= "optimization.pushRoundingMode(RoundingMode.TRUNCATE)";
  } else if (fname == "popRoundingMode") {
    *s+= "optimization.popRoundingMode()";
  }

  if (s->size() == 0) {
    LOG_CERR();
    cerr << "Function definition not found: '"+fname+"'" << endl;
  }

  return s;
}

void ASTtoMaxJVisitor::visitExprStmt(SgExprStatement *exprStmt) {
  D(cerr << "Visiing expression statment" << endl);
  string* s = toExpr(exprStmt->get_expression());
  if ( s != NULL )
    source += *s + ";\n";
}

void ASTtoMaxJVisitor::ignore(SgNode *n) {
  AstAttribute *ignore = new AstTextAttribute("yes");
  n->setAttribute("ignore", ignore);
}

void ASTtoMaxJVisitor::visitFor(SgForStatement *forStmt) {

  SgStatementPtrList initList = forStmt->get_init_stmt();
  SgStatement        *test = forStmt->get_test();
  SgExpression       *incr = forStmt->get_increment();

  source += "for (";
  foreach_(SgStatement* stmt, initList) {
    source += stmt->unparseToString();
    ignoreChildren(stmt);
  }
  source += test->unparseToString();
  source += incr->unparseToString();
  source += ")";
  ignoreChildren(test); ignoreChildren(incr);
  source += " {\n";
}

string ASTtoMaxJVisitor::constVar(string s) {
  return "constant.var(" + s + ")";
}

string ASTtoMaxJVisitor::constVar(string s, string type) {
  return constVar(type + "," + s);
}

bool ASTtoMaxJVisitor::isConstant(string s) {
  regex constant("-?[0-9]*(.[0-9]*)?");
  cmatch group;
  return regex_match(s.c_str(), group, constant);
}
