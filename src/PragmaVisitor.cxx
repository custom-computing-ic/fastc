#include "precompiled.hxx"
#include "PragmaVisitor.hxx"

#include <string>

using namespace std;

PragmaVisitor::PragmaVisitor(Design* design) {
    PRAGMA_IN = new regex("in (\\w*) (\\w*) (\\w*)");
    PRAGMA_OUT = new regex("out (\\w*) (\\w*) (\\w*)");
    PRAGMA_SCALAR_IN = new regex("scalar in (\\w*) (\\w*) (\\w*)");
    TYPE = new regex("([a-zA-Z]*)([0-9]*)(_([0-9]*))?");
    this->design = design;
}

void PragmaVisitor::visit(SgNode* n) {
    if (isSgPragma(n)) {
        visit(isSgPragma(n));
    }
}

string PragmaVisitor::getType(string t) {
    cmatch group;
    string type, exponent, mantissa, width;
    bool floatType = false;
    if (regex_match(t.c_str(), group, *TYPE)) {
        type =  group[1];
        if ( group[3].str().compare("") != 0 ) {
            exponent = group[2];
            mantissa = group[4];
            floatType = true;
        } else {
            width = group[2];
        }
    }

    if (type.compare("uint") == 0)
        type="hwUInt";
    else if (type.compare("int") == 0) {
        type="hwInt";
    } else if (type.compare("float") == 0) {
        type="hwFloat";
    }

    if (floatType)
        return type + "(" + exponent + "," + mantissa + ")";
    return type + "(" + width + ")";
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
        type = getType(sm[2]);
        Kernel* k = design->getKernel(sm[3]);
        k->addScalarInput(sm[1], type);
    }
}
