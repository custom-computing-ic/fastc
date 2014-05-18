#ifndef _INLINEKERNELS_HXX_
#define _INLINEKERNELS_HXX_

#include "../precompiled.hxx"
#include "Pass.hxx"

void FixSgProject(SgProject& proj);

class InlineKernels : public Pass
{
public:
  InlineKernels() {}

  void runPass(Design* design);

  int countFunctionCalls(SgFunctionDeclaration* decl);

  bool inlineFunctionCalls(SgFunctionDeclaration* decl, SgProject *project);

  string logPass();
};


#endif /* _INLINEKERNELS_HXX_ */
