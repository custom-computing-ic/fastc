#include "KernelExtraction.hxx"
#include "../utils.hxx"

void KernelExtraction::runPass(Design* design){
  KernelVisitor kernelVisitor(design);
  kernelVisitor.traverse(design->getProject(), preorder);
  foreach_(Kernel* k, design->getKernels()) {
    k->saveOriginalInputOutputNodes();
    cout << "\tFound Kernel: \033[1;31m" << k->getName() << "\033[0m" << endl;
    cout << "\t\tInputs: ";
    printList<string>(k->getInputNames());
    cout << endl <<  "\t\tOutputs: ";
    printList<string>(k->getOutputNames());
    cout << endl << "\t\tScalars: ";
    printList<string>(k->getScalarInputNames());
    cout << endl;
  }
  cout << endl;

}

string KernelExtraction::logPass() {
  return "Kernel Extraction";
}
