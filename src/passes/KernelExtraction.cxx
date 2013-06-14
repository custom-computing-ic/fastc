#include "KernelExtraction.hxx"

void KernelExtraction::runPass(Design* design){
  KernelVisitor kernelVisitor(design);
  kernelVisitor.traverse(design->getProject(), preorder);
  foreach_(Kernel* k, design->getKernels()){
    k->saveIO();
  }
}

string KernelExtraction::logPass() {
  return "Kernel Extraction";
}
