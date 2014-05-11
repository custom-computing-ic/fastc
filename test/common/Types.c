void kernel_Types(
                  float* sin1,
                  float* sout1
                  )
{
  // assign a stream to a stream
  float* svar1 = sin1;

  // create a hardware constant
  float var1 = 0.335;

  // create a MaxJ constant
  // XXX: this is not currently supported
  float const1 = 0.5;

  // multiply stream value by constant and assign result to value
  float inter = sin1[0] * var1;

  // create a new stream with the value of the intermediate result
  float* res;
  res[0] = inter;

  // output result stream by assigning to the declared output stream
  sout1[0] = res[0];
}
