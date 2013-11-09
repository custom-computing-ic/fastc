#include <maxcc.h>

kernel_MovingAverage(float *x, float *y)
{
   float prev = x[-1];
   float next = x[1];
   float sum = prev + x[0] + next;
   float result = sum / 3;
   y[0] = result;
}
