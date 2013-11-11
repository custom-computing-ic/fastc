#include "../../../include/maxcc.h"

void kernel_Convolution1d(
                          float* p,
                          float* output,
                          float c_0_0_0,
                          float c_p_0_0,
                          float c_n_0_0,
                          unsigned int n1,
                          unsigned int ORDER) {
  int i1 = count(32, n1, 1, NULL);
  int i4 = count(10, 1000, 1, i1);


  bool up = (i1 >= ORDER) && (i1 < n1 - ORDER);

#pragma class:kernelopt name:pushDSP factor:1
  float *result;
  result[0] =
    p[0]  * c_0_0_0 +
    p[1]  * c_p_0_0 +
    p[-1] * c_n_0_0;

  float* inter = up ? result : p;
#pragma class:kernelopt name:popDSP

  output[0] = inter[0];
}
