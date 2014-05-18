#ifndef _BUILDDFG_HXX_
#define _BUILDDFG_HXX_

#include "../precompiled.hxx"
#include "Pass.hxx"

class BuildDFG : public Pass {
public:

  BuildDFG(const Compiler& compiler) : super(compiler) {}

  void runPass(Design *design);
  string logPass();
};


#endif /* _BUILDDFG_HXX_ */
