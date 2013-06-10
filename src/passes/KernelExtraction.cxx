#include "KernelExtraction.hxx"

void KernelExtraction::runPass(Design* design){
  KernelVisitor kernelVisitor(design);
  kernelVisitor.traverse(design->getProject(), preorder);
}

string KernelExtraction::logPass() {
  return "Kernel Extraction";
}
