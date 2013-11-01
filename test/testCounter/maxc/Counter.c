#include "../../../include/maxcc.h"

void kernel_Counter(int *out) {

  // simple counter chain -- this uses the previous counter's wrap value
  int z_counter = count(7, 100, 1, NULL);
  int y_counter = count(7, 100, 1, z_counter);

  // using boolean stream -- this is used directly as enable signal
  bool enable = (z_counter == y_counter);
  int equals_counter = count(7, 100, 1, enable);

  out[0] = equals_counter + 1;
}
