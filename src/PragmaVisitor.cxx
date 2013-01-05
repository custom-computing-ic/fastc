#include "precompiled.hxx"
#include "PragmaVisitor.hxx"

#include <string>

using namespace std;

PragmaVisitor::PragmaVisitor(Design* design) {
	PRAGMA_IN = new regex("in (\\w*) (\\w*) (\\w*)");
	PRAGMA_OUT = new regex("out (\\w*) (\\w*) (\\w*)");
	PRAGMA_SCALAR_IN = new regex("scalar in (\\w*) (\\w*) (\\w*)");
	this->design = design;
}

void PragmaVisitor::visit(SgNode* n) {
	if (isSgPragma(n)) {
		visit(isSgPragma(n));
	}
}

void PragmaVisitor::visit(SgPragma* pragma) {
	string type("hwFloat(8, 24)");
	string s = pragma->get_pragma();
	cmatch sm;
	if (regex_match(s.c_str(), sm, *PRAGMA_IN)) {
		Kernel* k = design->getKernel(sm[3]);
		k->addInput(sm[1], type);
	} else if (regex_match(s.c_str(), sm, *PRAGMA_OUT)) {
		Kernel* k = design->getKernel(sm[3]);
		k->addOutput(sm[1], type);
	} else if (regex_match(s.c_str(), sm, *PRAGMA_SCALAR_IN)) {
		string t = type;
		if (sm[2].compare("uint32") == 0) {
			t = "hwUInt(32)";
		} else if (sm[2].compare("float8_24") == 0) {
			t = "hwFloat(8, 24)";
		}
		Kernel* k = design->getKernel(sm[3]);
		k->addScalarInput(sm[1], t);
	}
}
