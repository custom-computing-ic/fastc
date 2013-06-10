//#include "../../../include/maxcc.h"
//#include "CmdRead.c"
//#include "CmdWrite.c"
#include <math.h>

extern count_chain(int n1, int i, int i4);
extern count(int i1, int i2);

// TODO: Add pragma for accuracy
// TODO: Add pragma for parallelism
// TODO: Add pragma for bit width
void kernel_Convolution1d(
                          float* p,
                          float* output,
                          float c_0_0_0,
                          float c_p_0_0,
                          float c_n_0_0,
                          unsigned int n1,
                          unsigned int ORDER) {
  // TODO: replace counters with ++ and if/else
  //  int i4 = count(1000, 1);
  int i4 = sqrt(9);
  //  int i4 = 1;
  //   int i1 = count_chain(n1, 1, i4);
  /*  int i1 = 1;

    int* up;
    *up = (i1 >= ORDER) && (i1 < n1 - ORDER);


#pragma class:kernelopt name:pushDSP factor:1
    float* result;
    *result =
        p[0]  * c_0_0_0 +
        p[1]  * c_p_0_0 +
        p[-1] * c_n_0_0;

        output[0] = up ? result[0] : p[0];*/
}

void Price_CPU(){
  printf("Running CPU price\n");
}

int main(int argc, char *argv[])
{

  // TODO: Make link work
  // TODO: infer manager design
  // TODO: infer DRAM kernels
  // TODO: Add rest of CPU code
  // TODO: Generate CPU API
#pragma fast kernel:kernel_Convolution1d
  // #pragma fast kernel:kernel_Cmdread  type:mem_read  stream:p
  // #pragma fast kernel:kernel_Cmdwrite type:mem_write stream:result
  Price_CPU();

  return 0;
}
