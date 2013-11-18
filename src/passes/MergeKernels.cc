#include "../precompiled.hxx"
#include "MergeKernels.h"

void MergeKernels::runPass(Design *design) {

  // TODO which is the optimal partition? Should we generate all?
  if (design->getPartitions().empty())
    return;

  Partition* best = design->getPartitions()[0];

  // TODO what are the segments?
  foreach_(Configuration *c, best->getPartition()) {

    vector<Kernel*> kernels;
    foreach_(Segment *s, c->getConfiguration()){
      foreach_(DfeTask *d, s->getDfeTasks()) {
        kernels.push_back(d->getKernel());
      }
    }

    if (kernels.size() > 1) {
      // merge all kernels with the first one
      Kernel* kernel_to_merge = kernels[0];
      SgFunctionDeclaration* f_decl = kernel_to_merge->getDeclaration();
      SgScopeStatement* f_scope = f_decl->get_scope();
      SgBasicBlock* original = f_decl->get_definition()->get_body();

      for (int i = 1; i < kernels.size(); i++) {
        SgFunctionDeclaration* f_decl_to_merge = kernels[i]->getDeclaration();
        SgBasicBlock* block = f_decl_to_merge->get_definition()->get_body();
        SgStatement* st = block->get_statements()[1];
        SageInterface::appendStatement(st, original);
      }

    }

  }

}
