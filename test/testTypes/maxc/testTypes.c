#include "../../../include/maxcc.h"

#pragma dir:in name:sin1 type:float8_24 func:kernel_Types
void kernel_Types(
                  s_float8_24 sin1,
                  s_float8_24 sout1
                  )
{
    s_float8_24 svar1 = sin1;
    float8_24 var1 = 0.335;
    float   const1 = 0.5;

    float8_24 inter = sin1[0] * var1;

    s_float8_24 res;
    res[0] = inter;

    output_i(sout1, res);
}
