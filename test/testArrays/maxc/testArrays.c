#include "../../../include/maxcc.h"

void kernel_Arrays() {

    // an array of hw variables
    float8_24 hw_up[3];
    hw_up[0] = 1;
    hw_up[1] = 2;
    hw_up[2] = 3;

    // a multi-dim array of hw variables
    int32 hw_up3d[1][1][1];
    hw_up3d[0][0][0] = hw_up[0] + hw_up[1] + hw_up[2];

    // arrays of streams
    s_float8_24 p[2];
    p[0][0] = *(p[1]) + p[1][2] + hw_up3d[0][0][0];

    // multi-dim arrays of streams
    s_float8_24 pp[2][4];
    *(pp[0][1]) = pp[1][0][3] + *pp[0][1] + pp[0][1][2];
}
