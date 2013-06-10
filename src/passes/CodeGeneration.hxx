#ifndef CODE_GENERATION_HXX
#define CODE_GENERATION_HXX

#include "Pass.hxx"

class CodeGeneration : public Pass {
public:
  CodeGeneration() {}
  void runPass(Design* design);
  string logPass();
};

#endif
