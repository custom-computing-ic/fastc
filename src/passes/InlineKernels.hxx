#ifndef _INLINEKERNELS_HXX_
#define _INLINEKERNELS_HXX_

#include "../precompiled.hxx"
#include "Pass.hxx"

void FixSgProject(SgProject& proj);

class InlineKernels : public Pass
{
public:
  InlineKernels() {}

  void runPass(Design* design) {
    int iterCount = 0;
     foreach_(Kernel* k, design->getKernels()) {
       int bCalls, aCalls;
       bool inlined;
       do {
         iterCount++;
         bCalls = countFunctionCalls(k->getDeclaration());
         inlined = inlineFunctionCalls(k->getDeclaration(), design->getProject());
         aCalls = countFunctionCalls(k->getDeclaration());
       } while (bCalls > aCalls || inlined);
     }
     if (iterCount > 1)
       renameVariables(design->getProject());
  }

  int countFunctionCalls(SgFunctionDeclaration* decl) {
    Rose_STL_Container<SgNode*> calls;
    calls = NodeQuery::querySubTree(decl->get_definition(), V_SgFunctionCallExp);
    return calls.size();
  }

  bool inlineFunctionCalls(SgFunctionDeclaration* decl, SgProject *project) {
    Rose_STL_Container<SgNode*> calls;
    if (decl->get_definition() == NULL)
      return false;
    calls = NodeQuery::querySubTree(decl->get_definition(), V_SgFunctionCallExp);
    bool inlined = false;
    foreach_(SgNode* n, calls) {
      SgFunctionCallExp* fcall = isSgFunctionCallExp(n);
      if (fcall != NULL){
        inlined |= doInline(fcall);
      }
    }
    return inlined;
  }
  string logPass() {
    return "Inline Kernels";
  }
};


#endif /* _INLINEKERNELS_HXX_ */
