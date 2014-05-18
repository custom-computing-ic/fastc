#ifndef _INPUTOUTPUTEXTRACTION_HXX_
#define _INPUTOUTPUTEXTRACTION_HXX_

#include "Pass.hxx"

class InputOutputExtraction : public Pass
{
public:
  InputOutputExtraction(const Compiler& compiler) : super(compiler) {}
  void runPass(Design *design) {
    foreach_(Kernel *kernel, design->getKernels()) {
      kernel->extractIO(false);
    }
  }

  string logPass() {
    return "Output Extraction";
  }
};


#endif /* _INPUTOUTPUTEXTRACTION_HXX_ */
