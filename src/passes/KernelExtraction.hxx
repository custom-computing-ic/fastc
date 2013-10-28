#ifndef KERNEL_EXTRACTION_HXX
#define KERNEL_EXTRACTION_HXX

#include "../precompiled.hxx"
#include "KernelVisitor.hxx"
#include "Pass.hxx"

/** Extracts FAST dataflow kernels from the given design. */
class KernelExtraction : public Pass
{
public:
  KernelExtraction(){}
  void runPass(Design* design);
  string logPass();
};

#endif
