/**
 *  Simple example to demonstrate the use of maxc.
 *  1. We have a CPU and a dataflow implemntation for the passthrough function.
 *  2. We use a pragma to tell the compiler to which function we want to map the function call.
 *  3. Out/In streams are distinguished from assignments (i.e. we cannot assign to in streams)
 *  4. Example still compiles with regular gcc.
 */


#include <stdio.h>
#include <stdlib.h>
#include "../../../include/maxcc.h"

void PassThrough(int *x, int *y, int n) {
  int i;
  for (i = 0; i < n; i++)
    y[i] = x[i];
}

#define n 128

# pragma maxc dataflow
void kernel_PassThroughDF(int *x, int *y,
                          s_array_f8_24 in_array) {

  in_array = make_input_array_f(8, 24, 128);

  for (int size = 2; size <= n; size *= 2) {
    // TODO fill in the text
  }

  *y = *x * 2;
}

int main() {

  int size = 100, i;
  int *x = malloc(size * sizeof(int));
  int *y = malloc(size * sizeof(int));

  for (i = 0; i < n; i++ ) x[i] = i;

# pragma maxc func: PassThroughDF
  PassThrough(x, y, size);

  int failed = 0;
  for (i = 0; i < size; i++) {
    if (y[i] != i) {
      printf("Expected y[%d] = %d, got %d!", i, i, y[i]);
      failed = 1;
    }
  }

  return failed;
}
