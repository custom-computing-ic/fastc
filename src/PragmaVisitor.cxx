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

    if (type.compare("uint") == 0 || type.compare("s_uint") == 0 )
        type="hwUInt";
    else if (type.compare("int") == 0 || type.compare("s_int32") == 0)
        type="hwInt";
    else if (type.compare("float") == 0 || type.compare("s_float8_24") == 0)
        type="hwFloat";

    if (floatType)
        return type + "(" + exponent + "," + mantissa + ")";
    return type + "(" + width + ")";
}

void PragmaVisitor::visit(SgPragma* pragma) {

    string s = pragma->get_pragma();
    cmatch sm;
    string *cls   = get_param(pragma->get_pragma(), "class");
    string *dir   = get_param(pragma->get_pragma(), "dir");
    string *type  = get_param(pragma->get_pragma(), "type");
    string *width = get_param(pragma->get_pragma(), "width");
    string *name  = get_param(pragma->get_pragma(), "name");
    string *kernel = get_param(pragma->get_pragma(), "func");

    if  ( cls != NULL && cls->compare("kernelopt") == 0 )
	return;

    if ( kernel == NULL ) {
        cout << "Error : must specify kernel (func:<kerneL>) in pragma: ";
        //cout << pragma->get_pragma() << endl;
        // exit(1);
        return;
    }

    if (name == NULL) {
        cout << "Error : must specify input/output name in pragma: ";
        cout << pragma->get_pragma() << endl;
        exit(1);
    }

    Kernel *k = design->getKernel(*kernel);

    if ( k == NULL )  {
        cout << "Could not find specified kernel in the design!";
        cout << "Is kernel_" + *kernel + "defined?" << endl;
        exit(1);
    }

    if ( type != NULL )
        *type = getType(*type);
    /*
    if (cls == NULL) {
        if (dir->compare("in") == 0)
          k->addInput(*name, *type, "1");
        else if (dir->compare("out") == 0) {
          // XXX handled by inference, remove
          // k->addOutput(*name, *type);
        }
    } else if (cls->compare("scalar") == 0)
        k->addScalarInput(*name, *type);
    else if (cls->compare("array") == 0) {
        if (dir->compare("in") == 0)
            k->addInput(*name, *type, *width);
        else {
          // XXX handled by inference, remove
          //  k->addOutput(*name, *type, *width);
        }
        }*/
}

string* PragmaVisitor::get_param(string pragma, string param) {
    regex s(param + " : (\\w*)");
    cmatch sm;
    if ( regex_search(pragma.c_str(), sm, s) )
        return new string(sm[1]);
    return NULL;
}

bool PragmaVisitor::contains_param(string pragma, string param) {
    return pragma.find(param) != string::npos;
}
