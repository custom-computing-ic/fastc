#include "../../../include/maxcc.h"

void kernel_PassThroughKernel(int* x, int* y){
  y[0] = x[0];
}
