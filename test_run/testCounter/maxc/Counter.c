#include "../../../include/maxcc.h"

void kernel_Counter(int *out) {

  // simple counter chain -- this uses the previous counter's wrap value
  int z = count(7, 100, 1, NULL);
  int y = count(7, 100, 1, z);

  // using boolean stream -- this is used directly as enable signal
  bool enable = (z == y);
  int equals = count(7, 100, 1, enable);

  out[0] = equals + 1;
}
