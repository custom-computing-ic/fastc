#ifndef CODE_GENERATION_HXX
#define CODE_GENERATION_HXX

#include "Pass.hxx"

/** Generate MaxJ code corresponding to the FAST dataflow designs. */
class CodeGeneration : public Pass {
public:
  CodeGeneration(const Compiler& compiler) : super(compiler) {}
  void runPass(Design* design);
  string logPass();
};

#endif
