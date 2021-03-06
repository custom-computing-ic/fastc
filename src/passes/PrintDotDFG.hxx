#ifndef PRINT_DOT_DFG_HXX_
#define PRINT_DOT_DFG_HXX_

#include "Pass.hxx"
#include "../Design.hxx"

class PrintDotDFG : public Pass {

public:
  PrintDotDFG(const Compiler& compiler) : super(compiler) {}

  void runPass(Design *design);
  string logPass();

};

#endif
