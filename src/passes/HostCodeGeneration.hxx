#ifndef HOST_CODE_GENERATION_HXX
#define HOST_CODE_GENERATION_HXX

#include "Pass.hxx"
#include "../precompiled.hxx"



class HostCodeGeneration : public Pass
{


public:
  HostCodeGeneration() {}
  void runPass(Design* design) {
    design->getProject()->unparse();
  }

  string logPass() {
    return "Host Code Generation";
  }
};

#endif
