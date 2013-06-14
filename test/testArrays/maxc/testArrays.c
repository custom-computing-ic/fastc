#include "../../../include/maxcc.h"


const int Par = 2;

void kernel_Arrays(
                   float* array_in[Par],
                   float* array_out_1[Par],
                   float* array_out_2[Par]
) {

    // an array of hw variables
    float hw_up[3];
    hw_up[0] = 1;
    hw_up[1] = 2;
    hw_up[2] = 3;

    // a multi-dim array of hw variables
    int32 hw_up3d[1][1][1];
    hw_up3d[0][0][0] = hw_up[0] + hw_up[1] + hw_up[2];

    // arrays of streams
    float* p[2];
    p[0][0] = *(p[1]) + p[1][2] + hw_up3d[0][0][0];

    // multi-dim arrays of streams
    float* pp[2][4];
    *(pp[0][1]) = pp[1][0][3] + *pp[0][1] + pp[0][1][2];
    *(pp[0][2]) = pp[1][0][3] + *pp[0][1] + pp[0][1][2];

    //    array_out_1 = p;

    for (int i = 0; i < 2; i++)
      array_out_2[i][0] = pp[0][i][0];
}
