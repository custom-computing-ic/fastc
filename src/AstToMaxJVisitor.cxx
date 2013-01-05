#include "precompiled.hxx"
#include "AstToMaxJVisitor.hxx"

using namespace std;
using namespace boost;

class SgFunctionSymbol;
class SgVarRefExp;
class SgIntVal;
class SgBinaryOp;
class SgUnaryOp;

ASTtoMaxJVisitor::ASTtoMaxJVisitor() {
	currentKernel = NULL;
}

void ASTtoMaxJVisitor::visit(SgNode *n) {
	if (isSgVariableDeclaration(n)) {
		visit(isSgVariableDeclaration(n));
	} else if (isSgFunctionCallExp(n)) {
		visit(isSgFunctionCallExp(n));
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
		SgVarRefExp *dim = isSgVarRefExp(*itt);
		string d = "128";
		if (dim != NULL)
			d = dim->get_symbol()->get_name();

		SgIntVal *inc = isSgIntVal(*(++itt));
		string i;
		if (inc != NULL) {
			stringstream s;
			s << inc->get_value();
			i = s.str();
		}

		if (fname.compare("count") == 0) {
			// XXX it seems counter chains with one counter are supported?
			// TODO need a fresh variable for chain
			string name = "chain_" + variableName;
			declarations += "CounterChain " + name
					+ " = control.count.makeCounterChain();\n";
			source += "HWVar " + variableName + " = " + name + ".addCounter("
					+ d + ", " + i + ");\n";
			counterMap[variableName] = name;
		} else if (fname.compare("countChain") == 0) {
			string p = "";
			SgVarRefExp *parent = isSgVarRefExp(*(++itt));
			if (parent != NULL)
				p = parent->get_symbol()->get_name();
			string chainDeclaration = counterMap[p];
			source += "HWVar " + variableName + " = " + chainDeclaration
					+ ".addCounter(" + d + ", " + i + ");\n";
			counterMap[variableName] = chainDeclaration;
		}
	}

	if (fname.compare("fselect") == 0) {
		string *exp = toExpr(*itt);
		string *ifTrue = toExpr(*(++itt));
		string *ifFalse = toExpr(*(++itt));
		source += "HWVar " + variableName + " = control.mux(" + (*exp) + ", "
				+ (*ifTrue) + ", " + (*ifFalse) + ");\n";
	}

}

string* ASTtoMaxJVisitor::toExpr(SgExpression *ex) {
	if (isSgVarRefExp(ex)) {
		SgVarRefExp *e = isSgVarRefExp(ex);
		return new string(e->get_symbol()->get_name());
	} else if (isSgIntVal(ex)) {
		SgIntVal *e = isSgIntVal(ex);
		stringstream out;
		out << e->get_value();
		return new string("constant.var(" + out.str() + ")");
	} else if (isSgBinaryOp(ex)) {

		SgBinaryOp *e = isSgBinaryOp(ex);
		string *left = toExpr(e->get_lhs_operand());
		string *right = toExpr(e->get_rhs_operand());

		string op;
		if (isSgAddOp(ex))
			op = "+";
		else if (isSgMultiplyOp(ex))
			op = "*";
		else if (isSgAndOp(ex) || isSgBitAndOp(ex))
			op = "&";
		else if (isSgLessThanOp(ex))
			op = "<";
		else if (isSgGreaterThanOp(ex))
			op = ">";
		else if (isSgGreaterOrEqualOp(ex)) {
			op = ">=";
		} else if (isSgLessOrEqualOp(ex)) {
			op = "<=";
		}

		if (isSgPntrArrRefExp(ex)) {
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
				return new string(
						"stream.offset(" + (*left) + ", " + (*right)
								+ ", -1024, 1024)");
			}
		} else
			return new string("(" + (*left) + " " + op + " " + (*right) + ")");

	} else if (isSgUnaryOp(ex)) {
		SgUnaryOp *unOp = isSgUnaryOp(ex);
		string *exp = toExpr(unOp->get_operand());
		string op;
		if (isSgMinusOp(ex))
			op = "-";
		return new string(op + *exp);
	}

	return NULL;
}

void ASTtoMaxJVisitor::visit(SgVariableDeclaration* decl) {
	string type("hwFloat(8, 24)");
	SgVariableDeclaration *varDecl = isSgVariableDeclaration(decl);

	SgInitializedNamePtrList vars = varDecl->get_variables();
	SgInitializedNamePtrList::iterator it;

	for (it = vars.begin(); it != vars.end(); it++) {
		SgInitializedName *v = *it;
		string variableName = (*it)->get_qualified_name();

		SgInitializer *init = v->get_initializer();
		SgAssignInitializer *ass = isSgAssignInitializer(init);
		if (ass == NULL)
			continue;

		SgExpression *exp = ass->get_operand();

		// i = f() ..
		SgFunctionCallExp *fcall = isSgFunctionCallExp(exp);
		if (fcall != NULL)
			function_call_initializer(variableName, fcall);

		// i = expr;
		string* n = toExpr(exp);
		if (n != NULL)
			source += "HWVar " + variableName + " = " + (*n) + ";\n";
	}
}

void ASTtoMaxJVisitor::visit(SgFunctionCallExp *fcall) {
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
			cout << "NULL NAME!";
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
			cout << "NULL NAME!";
		} else {
			source += "io.output(\"" + (*name) + "\", " + (*expr) + ", " + type
					+ ");\n";
		}
	}
}
