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
    } if (*get_pragma_param(pragma, "ioType") != "") {
      string *ioType = get_pragma_param(pragma, "ioType");
      string *computeType = get_pragma_param(pragma, "computeType");
      kernel->updateTypeInfo(*var, *ioType, *computeType);
    }
  }

  void runPass(Design* design) {
    Rose_STL_Container<SgNode*> pragmas;
    pragmas = NodeQuery:: querySubTree(design->getProject(), V_SgPragma);
    foreach_(SgNode* pragma, pragmas) {
      SgPragma* p = isSgPragma(pragma);
      if ( p != NULL ) {
        string *k = get_pragma_param(p->get_pragma(), "func");
        Kernel *kernel = design->getKernel(*k);
        if (kernel != NULL)
          updateKernelDeclaration(kernel, p->get_pragma());
      }
    }
  }

  string logPass() {
    return "Pragma Extraction";
  }
};


#endif /* _PRAGMAEXTRACTION_HXX_ */
