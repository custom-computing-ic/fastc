#ifndef _PRAGMAEXTRACTION_HXX_
#define _PRAGMAEXTRACTION_HXX_

#include "Pass.hxx"
#include "../utils.hxx"

class PragmaExtraction : public Pass
{
public:
  PragmaExtraction(){}

  void updateKernelDeclaration(Kernel* kernel, string pragma) {
    string *var = get_pragma_param(pragma, "var");
    if (*get_pragma_param(pragma, "type") == "offset") {
      string *max = get_pragma_param(pragma, "max");
      string *min = get_pragma_param(pragma, "min");
      kernel->addOffsetExpression(*var, *max, *min);
    } 

    if (*get_pragma_param(pragma, "ioType") != "") {
      string *ioType = get_pragma_param(pragma, "ioType");
      string *computeType = get_pragma_param(pragma, "computeType");
      cout << "\t\tvar: '" << *var;
      if (ioType != NULL)
        cout << "' ioType: '" << *ioType;
      if (computeType != NULL)
        cout << "' computeType: " << *computeType << "'";
      cout << endl;
      kernel->updateTypeInfo(*var, *ioType, *computeType);
    }
  }

  void runPass(Design* design) {
    foreach_(Kernel* k, design->getKernels()) {
      SgFunctionDeclaration* fdecl = k->getDeclaration();
      vector<SgPragma*> pragmas;
      cout << "\tAnalysing annotated type info for kernel " << k->getName() << endl;
      SgStatement* stmt = SageInterface::getPreviousStatement(fdecl);
      SgPragmaDeclaration* p;
      while ((p = isSgPragmaDeclaration(stmt)) != NULL) {
        updateKernelDeclaration(k, p->get_pragma()->get_pragma());
        stmt = SageInterface::getPreviousStatement(stmt);
      }
    }
  }

  string logPass() {
    return "Pragma Extraction";
  }
};


#endif /* _PRAGMAEXTRACTION_HXX_ */
