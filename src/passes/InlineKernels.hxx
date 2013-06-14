#ifndef _INLINEKERNELS_HXX_
#define _INLINEKERNELS_HXX_

#include "../precompiled.hxx"
#include "Pass.hxx"

class InlineKernels : public Pass
{
public:
  InlineKernels() {}

  void runPass(Design* design) {
    foreach_(Kernel* k, design->getKernels()) {
      inlineFunctionCalls(k->getDeclaration());
    }
  }

  void inlineFunctionCalls(SgFunctionDeclaration* decl) {
    Rose_STL_Container<SgNode*> calls;
    if (decl->get_definition() == NULL)
      return;
    calls = NodeQuery::querySubTree(decl->get_definition(), V_SgFunctionCallExp);
    foreach_(SgNode* n, calls) {
      SgFunctionCallExp* fcall = isSgFunctionCallExp(n);
      if (fcall != NULL){
        SgFunctionDeclaration* decl = fcall->getAssociatedFunctionDeclaration();
        if (decl!= NULL)
          inlineFunctionCalls(decl);
        doInline(fcall);
      }
    }
  }
  string logPass() {
    return "Inline Kernels";
  }
};


#endif /* _INLINEKERNELS_HXX_ */
