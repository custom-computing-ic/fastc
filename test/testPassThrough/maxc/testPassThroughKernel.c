#include "../../../include/maxcc.h"

#pragma dir:in name:x type:int32 func:kernel_PassThroughKernel
void kernel_PassThroughKernel(
                        s_int32 x,
                        s_int32 y
                        ){
    output_i(y, *x);
}
