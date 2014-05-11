void kernel_Scalars(float *in_stream, float in_scalar, float *out_stream) {
  out_stream[0] = in_scalar * in_stream[0];
}
