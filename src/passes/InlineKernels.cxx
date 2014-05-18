#include "InlineKernels.hxx"

void InlineKernels::runPass(Design* design) {
    int iterCount = 0;
     foreach_(Kernel* k, design->getKernels()) {
       int bCalls, aCalls;
       bool inlined;
       do {
         iterCount++;
         bCalls = countFunctionCalls(k->getDeclaration());

	 if (bCalls == 0)
	   return;

         inlined = inlineFunctionCalls(k->getDeclaration(), design->getProject());
         aCalls = countFunctionCalls(k->getDeclaration());
       } while (bCalls > aCalls || inlined);
     }
     if (iterCount > 1)
       renameVariables(design->getProject());
}

int InlineKernels::countFunctionCalls(SgFunctionDeclaration* decl) {
    Rose_STL_Container<SgNode*> calls;
    calls = NodeQuery::querySubTree(decl->get_definition(), V_SgFunctionCallExp);
    return calls.size();
}

bool InlineKernels::inlineFunctionCalls(SgFunctionDeclaration* decl, SgProject *project) {
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

string InlineKernels::logPass() {
  return "Inline Kernels";
}
