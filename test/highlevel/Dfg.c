void kernel_Dfg (int* a, int *b) {
  int *c;
  c = a[0] + 2 + 2 * a[1];
  b[0] = a[0] + 2 + c;
}
