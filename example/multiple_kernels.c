#include <stdio.h>

void AddOne(int *x, int *y) {
  *y = *x + 1;
}

void Square(int *x, int *y) {
  *y = *x * *x;
}

int main() {

  int n = 100;

  int *x = malloc(n * sizeof(int));
  int *y = malloc(n * sizeof(int));

  for (i = 0; i < n; i++) x[i] = i;

  // create the two kernel instances
  kernel_t k1 = kernel(&AddOne);
  kernel_t k2 = kernel(&Square);

  // connect output y of AddOne to input x of Square
  connect(k1, k2, "y", "x");

  // set cycle count
  set_cycles(k1, n);
  set_cycles(k2, n);

  // run design
  run(); // or run(k1, k2)

  return 0;
}
