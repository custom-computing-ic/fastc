#include "MergeKernels.h"

void MergeKernels::runPass(Design *design) {

  // TODO which is the optimal partition? Should we generate all?
  if (design->getPartitions().empty())
    return;

  Partition* best = design->getPartitions()[0];

  // TODO what are the segments?
  foreach_(Configuration *c, best->getPartition()) {
    Kernel *merged_kernel = new Kernel();

    foreach_(Segment *s, c->getConfiguration()){
      foreach_(DfeTask *d, s->getDfeTasks()) {
        Kernel *kernel_to_merge = d->getKernel();
        merged_kernel->mergeKernel(kernel_to_merge);
      }
    }

    c->setMergedKernel(merged_kernel);
  }

}
