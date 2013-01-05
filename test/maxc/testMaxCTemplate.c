typedef int stream_t;
typedef int scalar_array;
typedef int count_t;

//void input(stream_t *x, int dim) {}
void output(stream_t* out, int res, int cond) {}
void chain(count_t i, count_t j, count_t k) {}
int fselect(int cond, int val1, int val2) {return (cond ==0? val1 : val2);}
count_t count(int wrap, int inc) {return 1;}
count_t countChain(int wrap, int inc, count_t parent) {return 1;}
int scalar(){return 1;}
//void input_scalar_array(scalar_array *c, int count) {}

#pragma in x CPU kernel_KernelFunc
#pragma out res CPU kernel_KernelFunc
#pragma scalar in c float8_24 kernel_KernelFunc
#pragma scalar in a1 uint32 kernel_KernelFunc
#pragma scalar in a2 uint32 kernel_KernelFunc
#pragma scalar in a3 uint32 kernel_KernelFunc
#pragma scalar in n1 uint32 kernel_KernelFunc
#pragma scalar in n2 uint32 kernel_KernelFunc
#pragma scalar in n3 uint32 kernel_KernelFunc
#pragma scalar in nx uint32 kernel_KernelFunc
#pragma scalar in ny uint32 kernel_KernelFunc
#pragma scalar in nz uint32 kernel_KernelFunc
void kernel_KernelFunc(stream_t *x, stream_t *res,
		 int nx, int ny, int nz, 
		 int a1, int a2, int a3,
		 int n1, int n2, int n3, 
		 int c) {

  // counters are required to keep track of point being 
  // processed and determine if this is within boundaries
  count_t i = count(nz, 1);
  count_t j = countChain(ny, 1, i);
  count_t k = countChain(nx, 1, j);

  // keep track if we are inside boundaries
  int cond = (i >= a1 & i < n1) & (j >= a2 & j < n2) & (k >= a3 & k < n3);

  int func =
          c * x[0] + 
          c * x[1]      + c * x[-1] +
          c * x[-nx]    + c * x[nx] + 
          c * x[-nx*ny] + c * x[nx*ny];


  // if inside boundaries apply the stencil operator
  // otherwise assign a default value (0)
  // int r = fselect(cond, func, 0);

  output(res, func, cond);
}
