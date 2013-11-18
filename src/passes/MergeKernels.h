#ifndef MERGEKERNELS_H_
#define MERGEKERNELS_H_

#include "Pass.hxx"
#include "../Design.hxx"

class MergeKernels : public Pass {

 public:

  void runPass(Design *design);
  std::string logPass() { return "Merge Kernels"; }
};

#endif
