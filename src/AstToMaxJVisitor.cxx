#include "AstToMaxJVisitor.hxx"

using namespace std;
using namespace boost;

ASTtoMaxJVisitor :: ASTtoMaxJVisitor() {
  PRAGMA_IN     = new regex("in (\\w*) (\\w*) (\\w*)");
  PRAGMA_OUT    = new regex("out (\\w*) (\\w*) (\\w*)");
  PRAGMA_SCALAR_IN = new regex("scalar in (\\w*) (\\w*)");
  KERNEL_FUNC = new regex("kernel_(.*)");
  declarations = declaration() + "\n";
}

void ASTtoMaxJVisitor::function_call_initializer(
		string& variableName,
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
			declarations += "CounterChain "+name+" = control.count.makeCounterChain();\n";
			source += "HWVar "+variableName+" = "+name+".addCounter("+d+", "+i+");\n";
			counterMap[variableName] = name;
		} else if (fname.compare("countChain") == 0) {
			string p = "";
			SgVarRefExp *parent = isSgVarRefExp(*(++itt));
			if (parent != NULL)
				p = parent->get_symbol()->get_name();
			string chainDeclaration = counterMap[p];
			source += "HWVar "+variableName+" = "+chainDeclaration+".addCounter("+d+", "+i+");\n";
			counterMap[variableName] = chainDeclaration;
		}
	}

	if (fname.compare("fselect") == 0) {
		string *exp = toExpr(*itt);
		string *ifTrue = toExpr(*(++itt));
		string *ifFalse = toExpr(*(++itt));
		source += "HWVar "+variableName+" = control.mux("+(*exp)+", "+(*ifTrue)+", "+(*ifFalse)+");\n";
	}

}

string* ASTtoMaxJVisitor :: toExpr(SgExpression *ex) {
  if ( isSgVarRefExp(ex) ) {
    SgVarRefExp *e = isSgVarRefExp(ex);
    return new string(e->get_symbol()->get_name());
  } else if (isSgIntVal(ex) )  {
    SgIntVal *e = isSgIntVal(ex);
    stringstream out;
    out << e->get_value();
    return new string("constant.var("+out.str()+")");
  } else if (isSgBinaryOp(ex)) {

    SgBinaryOp *e = isSgBinaryOp(ex);
    string *left  = toExpr(e->get_lhs_operand());
    string *right = toExpr(e->get_rhs_operand());

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
    else if (isSgGreaterOrEqualOp(ex)) {
      op = ">=";
    } else if (isSgLessOrEqualOp(ex)) {
      op = "<=";
    }

    if ( isSgPntrArrRefExp(ex)) {
    	// XXX optimisation should be done in a separate pass
    	if ((*right).compare("0") == 0)
    		return left;

    	SgIntVal* rhs = isSgIntVal(e->get_rhs_operand());
    	if (rhs != NULL) {
    		stringstream out;
    		out << rhs->get_value(); //XXX this should be reused
        	return new string("stream.offset("+(*left)+", "+out.str()+")");
    	} else {
    		// XXX limits for dynamic offsets should be inferred
        	return new string("stream.offset("+(*left)+", "+(*right)+", -1024, 1024)");
    	}
    } else
    	return new string("("+(*left)+" "+op+" "+(*right)+")") ;

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

// XXX this implementation does not support forward declarations
/*void ASTtoMaxJVisitor:: visit(SgFunctionDeclaration* functionDeclaration) {

	string funcName = functionDeclaration->get_name().getString();
	cmatch sm;
	if (regex_match(funcName.c_str(), sm, *KERNEL_FUNC)) {
		string kernelName = sm[1].str();
		cout << "Found kernel -- " << kernelName << endl;
		Kernel* k = new Kernel(kernelName);
		kernels.push_back(k);
		currentKernel = k;
		//visit(functionDeclaration->get_definition()); // visit the function body
	}
}*/

void ASTtoMaxJVisitor :: visit(SgVariableDeclaration* decl) {
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

void ASTtoMaxJVisitor :: visit(SgPragma* pragma) {
	string type("hwFloat(8, 24)");
	string s = pragma->get_pragma();
	cmatch sm;
	if (regex_match(s.c_str(), sm, *PRAGMA_IN)) {
		declarations += "HWVar " + sm[1] + " =  io.input(\"" + sm[1] + "\", "
				+ type + ");\n";
	} else if (regex_match(s.c_str(), sm, *PRAGMA_OUT)) {
		//source += "HWVar "+sm[1]+" =  io.output(\""+sm[1]+"\","+type+");\n";
	} else if (regex_match(s.c_str(), sm, *PRAGMA_SCALAR_IN)) {
		string t = type;
		if (sm[2].compare("uint32") == 0) {
			t = "hwUInt(32)";
		} else if (sm[2].compare("float8_24") == 0) {
			t = "hwFloat(8, 24)";
		}
		declarations += "HWVar " + sm[1] + " =  io.scalarInput(\"" + sm[1]
				+ "\", " + t + ");\n";
	}
}

void ASTtoMaxJVisitor :: visit(SgFunctionCallExp *fcall) {
	string type("hwFloat(8, 24)");
	SgExpressionPtrList args = fcall->get_args()->get_expressions();
	SgExpressionPtrList::iterator it = args.begin();
	SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
	string fname = fsymbol->get_name();
	// output()
	if (fname.compare("output") == 0) {
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
	}
}

void ASTtoMaxJVisitor :: atTraversalStart() {
	cout << "At start!";
	declarations += declaration() + "\n";
}

void ASTtoMaxJVisitor :: atTraversalEnd() {
	cout << "At end!";
	source += "}\n}\n";
	//cout << declarations + "\n\n" + source << endl;
}

string ASTtoMaxJVisitor :: imports() {
	string imps[] = {
			"Kernel" ,
			"KernelParameters",
			"types.base.HWVar",
			//"stdlib.core.Stream.OffsetExpr",
			"stdlib.core.CounterChain"};
	list<string> imports(imps, imps + 4);
	return import(imports);

}

string ASTtoMaxJVisitor:: import(list<string>  imports) {
	string import = "";
	list<string>::iterator it;
	for (it = imports.begin(); it != imports.end(); it++)
		import += "import com.maxeler.maxcompiler.v1.kernelcompiler." + (*it) +";\n";
	return import;
}

string ASTtoMaxJVisitor :: kernelName() {
	return "KernelFunc";
}

string ASTtoMaxJVisitor :: declaration() {
	  return  "package engine;\n\n" + imports() + "\n\n"
			  "public class " + kernelName() + " extends Kernel {\n\n"
	  	  	  "public " + kernelName() + "(KernelParameters parameters) {\n"
	  	  	  "super(parameters);";
}

void ASTtoMaxJVisitor :: writeKernels(ostream& out) {
	out << getSource() << "}\n}\n";
}
