//#include <iostream>

/** 
 * This test shows how to create a simple working example 
 *  of integrated FAST + C code. Data dependency between kernels
 *  is captured in the hierarchical DFG
 */


void kernel_f1(int *a, int *b) {
  b[0] = *a + 1;
}

void kernel_f2(int *a, int *b) {
  b[0] = *a * 5;
}

void kernel_f3(int * a, int *b, int *c) {
  c[0] = *a  + *b;
}

int main() {

  int size = 384;
  int *in = (int *)malloc(sizeof(int) * size);

  for (int i = 0; i < 10; i++)
    in[i] = 0;
  
  int *out1 = (int *)malloc(sizeof(int) * size);
  int *out2 = (int *)malloc(sizeof(int) * size);
  int *out3 = (int *)malloc(sizeof(int) * size);
  int *out4 = (int *)malloc(sizeof(int) * size);

#pragma fast hw:kernel_f1
  kernel_f1(in, out1);
  
#pragma fast hw:kernel_f2
  kernel_f2(out1, out2);

#pragma fast hw:kernel_f3
  kernel_f3(out1, out2, out3);

#pragma fast hw:kernel_f3
  kernel_f3(out1, out3, out4);

  for (int i = 0; i < 10; i++)
    if (out3[i] != 6) {
      //      cout << "Error" << endl;
      return 1;
    }
  
  return 0;
}
