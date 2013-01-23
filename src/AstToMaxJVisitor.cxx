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
    currentKernel = NULL;
    paramCount = 0;
}

void ASTtoMaxJVisitor::preOrderVisit(SgNode *n) {

    D(cerr << "Visiting node: " << n->unparseToString());
    D(cerr << ",ignore: " << n->getAttribute("ignore") << endl);

    if ( n->getAttribute("ignore") != NULL )
        return;

    if (isSgVariableDeclaration(n))
        visitVarDecl(isSgVariableDeclaration(n));
    else if (isSgFunctionCallExp(n))
        visitFcall(isSgFunctionCallExp(n));
    else if (isSgForStatement(n))
        visitFor(isSgForStatement(n));
    else if (isSgExprStatement(n))
        visitExprStmt(isSgExprStatement(n));

}

void ASTtoMaxJVisitor::postOrderVisit(SgNode *n) {
    if (isSgForStatement(n)) {
        source += "\n}\n";
    }
}

void ASTtoMaxJVisitor::function_call_initializer(string& variableName,
                                                 SgFunctionCallExp *fcall) {

    SgExpressionPtrList args = fcall->get_args()->get_expressions();
    SgExpressionPtrList::iterator itt;
    itt = args.begin();
    SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
    string fname = fsymbol->get_name();
    if (fname.compare("count") == 0 || fname.compare("countChain") == 0) {
        string *wrap = toExpr(*itt);

        SgIntVal *inc = isSgIntVal(*(++itt));
        string i;
        if (inc != NULL) {
            stringstream s;
            s << inc->get_value();
            i = s.str();
        }

        if (fname.compare("count") == 0) {
            D(cerr << "Visiting count " << endl);
            // XXX it seems counter chains with one counter are supported?
            // TODO need a fresh variable for chain
            string name = "chain_" + variableName;
            declarations += "CounterChain " + name
                + " = control.count.makeCounterChain();\n";
            source += "HWVar " + variableName + " = " + name + ".addCounter("
                + *wrap + ", " + i + ");\n";
            counterMap[variableName] = name;
        } else if (fname.compare("countChain") == 0) {
            string p = "";
            SgVarRefExp *parent = isSgVarRefExp(*(++itt));
            if (parent != NULL)
                p = parent->get_symbol()->get_name();
            string chainDeclaration = counterMap[p];
            source += "HWVar " + variableName + " = " + chainDeclaration
                + ".addCounter(" + *wrap + ", " + i + ");\n";
            counterMap[variableName] = chainDeclaration;
        }
    }

    if (fname.compare("count_p") == 0) {
        string param = "param" + to_string(paramCount);
        string *width = toExpr(*(itt));
        string *max   = toExpr(*(++itt));
        string *inc   = toExpr(*(++itt));
        string *enable = toExpr(*(++itt));
        source += "Count.Params " + param + " = control.count.makeParams(" + *width + ")"
            + ".withMax(" + *max + ")"
            + ".withInc(" + *inc + ")";
        if (enable != NULL)
            source += ".withEnable(" + *enable + ")";
        source += ";\n";
        string counter = "counter" + to_string(paramCount);
        source += "Counter " + counter + " = control.count.makeCounter(" + param + ");\n";
        source += "HWVar " + variableName + " = " + counter + ".getCount();\n";
        paramCount++;
    } else if (fname.compare("fselect") == 0) {
        string *exp = toExpr(*itt);
        string *ifTrue = toExpr(*(++itt));
        string *ifFalse = toExpr(*(++itt));
        source += "HWVar " + variableName + " = control.mux(" + (*exp) + ", "
            + (*ifTrue) + ", " + (*ifFalse) + ");\n";
    } else if (fname.compare("make_offset") == 0) {
        string *min  = toExpr(*itt);
        string *max  = toExpr(*(++itt));
        source += "OffsetExpr " + variableName;
        source += " = stream.makeOffsetParam("+ variableName + "," + *min + "," + *max + ");\n";
    }
}

string* ASTtoMaxJVisitor::toExpr(SgExpression *ex) {
    if (isSgVarRefExp(ex)) {
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
    } else if (isSgBinaryOp(ex)) {

        SgBinaryOp *e = isSgBinaryOp(ex);

        string op;

        string *left;
        if (isSgAssignOp(ex)) {
            op = "=";
            left = new string(e->get_lhs_operand()->unparseToString());
        } else {
            left = toExpr(e->get_lhs_operand());
        }

        string *right = toExpr(e->get_rhs_operand());

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
        else if (isSgAssignOp(ex))
            op = "=";
        else if (isSgDivideOp(ex))
            op = "/";
        if (isSgEqualityOp(ex))
            return new string((*left) + ".eq(" + (*right) + ")");

        if (isSgPntrArrRefExp(ex)) {
            SgPntrArrRefExp *e = isSgPntrArrRefExp(ex);
            // check for the type of lhs
            string type =  e->get_type()->unparseToString();
            D(cerr << "PTR REF: Found ptr ref type " << type);
            D(cerr << "Expr: " << ex->unparseToString() << endl);
            D(cerr << "PTR REF: Type of lhs" <<  e->get_lhs_operand()->get_type()->unparseToString() << endl);
            string lhs_type = e->get_lhs_operand()->get_type()->unparseToString();
            if (lhs_type.compare("s_float8_24") == 0 ||
                lhs_type.compare("s_int32") == 0) {
		// LHS is stream type so pointer access maps to stream.offset()

                // XXX optimisation should be done in a separate pass
                if ((*right).compare("0") == 0)
                    return left;

                SgIntVal* rhs = isSgIntVal(e->get_rhs_operand());
                if (rhs != NULL) {
                    stringstream out;
                    out << rhs->get_value(); //XXX this should be reused
                    return new string(
                                      "stream.offset(" + (*left) + ", " + out.str() + ")");
                } else {
                    // XXX limits for dynamic offsets should be inferred
                    // first argument to offset() is HWVar
                    string val = *right;
                    if (isConstant(*right))
                        val = constVar(val);
                    return new  string("stream.offset(" + (*left) + ", " + val + ", -1024, 1024)");
                }
            }

            SgArrayType *t  = isSgArrayType(e->get_type());
            if (t != NULL) {
                vector<SgExpression*> expressions = SageInterface::get_C_array_dimensions(t);
                vector<SgExpression*>::iterator it;
                int dim = expressions.size() - 1;
                D(cerr << "Array type dim: " << dim << endl);

                for (SgExpression* d : expressions ) {
                    string *value = toExpr(d);
                    if ( value != NULL )
                        cerr << "Extra args: " + *value + "]";
                }

                if (dim >= 1) {
                    return new string(ex->unparseToString());
                }
            } else {
		return new string(ex->unparseToString());
	    }

            D(cerr << "PTR REF: No type found for exp: " << e->unparseToString());

        } else
            return new string("(" + (*left) + " " + op + " " + (*right) + ")");

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

        return new string(op + *exp);
    }

    return NULL;
}

void ASTtoMaxJVisitor::visitVarDecl(SgVariableDeclaration* decl) {

    SgVariableDeclaration *varDecl = isSgVariableDeclaration(decl);

    SgInitializedNamePtrList vars = varDecl->get_variables();

    SgInitializedNamePtrList::iterator it;
    for (it = vars.begin(); it != vars.end(); it++) {
        SgInitializedName *v = *it;
        string variableName = v->get_qualified_name();

        if (isSgArrayType(v->get_type()) ) {
            SgArrayType *t  = isSgArrayType(v->get_type());

            vector<SgExpression*> expressions = SageInterface::get_C_array_dimensions(t);
            vector<SgExpression*>::iterator it;
            int dim = expressions.size() - 1;

            source += "HWVar " + variableName;
            for (int i = 0 ; i < dim; i++ ) {
                source += "[]";
            }
            source += " = new HWVar ";
            for (SgExpression* d : expressions ) {
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
        if (fcall != NULL)
            function_call_initializer(variableName, fcall);

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

void ASTtoMaxJVisitor::visitFcall(SgFunctionCallExp *fcall) {
    cerr << "Handling fcall " << endl;
    string type("hwFloat(8, 24)");
    SgExpressionPtrList args = fcall->get_args()->get_expressions();
    SgExpressionPtrList::iterator it = args.begin();
    SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
    string fname = fsymbol->get_name();
    // output()
    if (fname.compare("output_ic") == 0) {
        string* name = toExpr(*it);
        string* expr = toExpr(*(++it));
        string* cond = toExpr(*(++it));
        if (name == NULL || expr == NULL) {
            cerr << "NULL NAME!";
        } else if (cond == NULL)
            source += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
                + ");\n";
        else {
            source += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
                + ", " + (*cond) + ");\n";
        }
    } else if (fname.compare("output_i") == 0) {
        string* name = toExpr(*it);
        string* expr = toExpr(*(++it));
        if (name == NULL || expr == NULL) {
            cerr << "NULL NAME!";
        } else {
            source += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
                + ");\n";
        }
    } else if (fname.compare("DRAMOutput") == 0) {
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

        source += "DRAMCommandStream.makeKernelOutput("
            + *streamName + ",\n"
            + *control + ",\n"
            + *address + ",\n"
            + constVar(*size, "hwUInt(8)") + ",\n"
            + constVar(*inc, "hwUInt(6)") + ",\n"
            + constVar(*streamNo, "hwUInt(4)") + ",\n"
            + *interrupt
            + ");\n";

    } else if (fname.compare("pushDSPFactor") == 0) {
        string *exp = toExpr(*it);
        source += "optimization.pushDSPFactor(" + *exp + ");\n";
    } else if (fname.compare("popDSPFactor") == 0 ) {
        source += "optimization.popDSPFactor();\n";
    }
}

void ASTtoMaxJVisitor::visitExprStmt(SgExprStatement *exprStmt) {
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

    SgStatementPtrList::iterator it;
    for (it = initList.begin(); it != initList.end(); it++ ) {
        SgStatement* stmt = *it;
        source += stmt->unparseToString();
        ignore(stmt);
    }

    source += test->unparseToString();
    source += incr->unparseToString();
    source += ")";

    ignore(test); ignore(incr);

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
