#ifndef _INPUTOUTPUTEXTRACTION_HXX_
#define _INPUTOUTPUTEXTRACTION_HXX_

#include "Pass.hxx"

class InputOutputExtraction : public Pass
{
public:
  InputOutputExtraction() {}
  void runPass(Design *design) {
    foreach_(Kernel *kernel, design->getKernels()) {
      kernel->extractIO();
    }
  }

  string logPass() {
    return "Output Extraction";
  }
};


#endif /* _INPUTOUTPUTEXTRACTION_HXX_ */