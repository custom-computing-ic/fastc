#include "KernelVisitor.hxx"
#include "../AstToMaxJVisitor.hxx"

#include <string>

using namespace std;
using namespace boost;

KernelVisitor::KernelVisitor(Design *design) {
  KERNEL_FUNC =  new regex("kernel_(.*)");
  this->design = design;
}

void KernelVisitor :: visit (SgNode* n) {
  SgFunctionDeclaration* decl;
  if ( (decl = isSgFunctionDeclaration(n)) != NULL) {
    string funcName = decl->get_name().getString();
    cmatch sm;
    if (regex_match(funcName.c_str(), sm, *KERNEL_FUNC)) {
      string kernelName = sm[1].str();
      Kernel* k = new Kernel(kernelName, decl);
      design->addKernel(k);
    }
  }

};

void KernelVisitor :: writeKernels(ostream& out) {
  list<Kernel*>::iterator it;
  for (it = kernels.begin(); it != kernels.end(); it++ ) {
    out << (*it)->generateSourceCode();
    out << "\n\n";
  }
}
