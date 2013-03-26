/**
 *  Simple example to demonstrate the use of maxc.
 *  1. We have a CPU and a dataflow implemntation for the passthrough function.
 *  2. We use a pragma to tell the compiler to which function we want to map the function call.
 *  3. Out/In streams are distinguished from assignments (i.e. we cannot assign to in streams)
 *  4. Example still compiles with regular gcc.
 */


#include <stdio.h>
#include <stdlib.h>

void PassThrough(int *x, int *y, int n) {
  int i;
  for (i = 0; i < n; i++)
    y[i] = x[i];
}

# pragma maxc dataflow
void kernel_PassThroughDF(int *x, int *y) {
  *y = *x;
}

int main() {

  int n = 100, i;
  int *x = malloc(n * sizeof(int));
  int *y = malloc(n * sizeof(int));

  for (i = 0; i < n; i++ ) x[i] = i;

# pragma maxc func: PassThroughDF
  PassThrough(x, y, n);

  int failed = 0;
  for (i = 0; i < n; i++) {
    if (y[i] != i) {
      printf("Expected y[%d] = %d, got %d!", i, i, y[i]);
      failed = 1;
    }
  }

  return failed;
}
