const int Par;

int stream_diff(float *value[Par], int offset, int pipe) {
  int cycle_offset = (pipe + offset) / Par;
  int pipe_offset  = (pipe + offset) % Par;
  DFEVar arr1 = stream.offset(value[pipe_offset], cycle_offset);

  int cycle_offset_neg = (Par - 1 - pipe + offset) / Par;
  int pipe_offset_neg  = Par - 1 - (Par - 1 - pipe + offset) % Par;
  DFEVar arr2 = stream.offset(value[pipe_offset_neg], -cycle_offset_neg);
  return arr2 - arr1;
}

int diff(float *value[Par], float h, int pipe) {
  return (-2 * stream_diff(value, 2, pipe)
          -1 * stream_diff(value, 1, pipe)) / (10 * h);
}

void kernel_SGDiff( float* value[Par], int width, int size, double step){
  int cycle = Par * CYCLE_COUNT + pipe;
  bool compute = (cycle >= width) & (cycle < size - width);

  for (int pipe = 0; pipe < Par; pipe++) {
    result[pipe] <== compute ? diff(value, h, pipe) : value[pipe]
  }
}
