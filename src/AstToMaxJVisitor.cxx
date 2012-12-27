#include "precompiled.hxx"

#include "AstToMaxJVisitor.hxx"
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>

#include "DataFlowGraph/Node.hxx"
#include "DataFlowGraph/InputNode.hxx"
#include "DataFlowGraph/OutputNode.hxx"
#include "DataFlowGraph/DataFlowGraph.cxx"

using namespace std;
using namespace boost;

ASTtoMaxJVisitor :: ASTtoMaxJVisitor() {
  PRAGMA_IN     = new regex("in (\\w*) (\\w*) (\\w*)");
  PRAGMA_OUT    = new regex("out (\\w*) (\\w*) (\\w*)");
  PRAGMA_SCALAR_IN = new regex("scalar in (\\w*) (\\w*)");
  KERNEL_FUNC = new regex("kernel_.*");
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
  return toExprRec(ex);
}

string* ASTtoMaxJVisitor :: toExprRec(SgExpression *ex) {
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
    string *left  = toExprRec(e->get_lhs_operand());
    string *right = toExprRec(e->get_rhs_operand());

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
    string *exp = toExprRec(unOp->get_operand());
    string op;
    if (isSgMinusOp(ex))
      op = "-";
    return new string(op + *exp);
  }

  return NULL;
}

void ASTtoMaxJVisitor :: visit(SgNode *n) {

	string type("hwFloat(8, 24)");

	if (isSgVariableDeclaration(n)) {
		SgVariableDeclaration *varDecl = isSgVariableDeclaration(n);

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
				source += "HWVar "+variableName+" = "+(*n)+";\n";
		}

	} else if (isSgPragma(n)) {
		SgPragma* pragma = (SgPragma *) n;
		string s = pragma->get_pragma();
		cmatch sm;
		if (regex_match(s.c_str(), sm, *PRAGMA_IN)) {
			declarations += "HWVar "+sm[1]+" =  io.input(\""+sm[1]+"\", "+type+");\n";
		} else if (regex_match(s.c_str(), sm, *PRAGMA_OUT)) {
			//source += "HWVar "+sm[1]+" =  io.output(\""+sm[1]+"\","+type+");\n";
		} else if (regex_match(s.c_str(), sm, *PRAGMA_SCALAR_IN)) {
			string t = type;
			if (sm[2].compare("uint32") == 0) {
				t = "hwUInt(32)";
			} else if (sm[2].compare("float8_24") == 0) {
				t ="hwFloat(8, 24)";
			}
			declarations += "HWVar "+sm[1]+" =  io.scalarInput(\""+sm[1]+"\", "+t+");\n";
		}
	} else if (isSgFunctionCallExp(n)) {
		SgFunctionCallExp *fcall = isSgFunctionCallExp(n);
		SgExpressionPtrList args = fcall->get_args()->get_expressions();
		SgExpressionPtrList::iterator it;
		it = args.begin();
		SgFunctionSymbol* fsymbol = fcall->getAssociatedFunctionSymbol();
		string fname = fsymbol->get_name();
		// output()
		if (fname.compare("output") == 0) {
			string* name = toExpr(*it);
			string* expr = toExpr(*(++it));
			string* cond = toExpr(*(++it));
			if ( name == NULL || expr == NULL) {
				cout << "NULL NAME!";
			} else if (cond == NULL)
				source += "io.output(\""+(*name)+"\", "+(*expr)+", "+type+");\n";
			else {
				source += "io.output(\""+(*name)+"\", "+(*expr)+", "+type+", "+(*cond)+");\n";
			}
		}
	}

}

void ASTtoMaxJVisitor :: atTraversalStart() {
	declarations += declaration() + "\n";
}

void ASTtoMaxJVisitor :: atTraversalEnd() {
	source += "}\n}\n";
	cout << declarations + "\n\n" + source << endl;
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
