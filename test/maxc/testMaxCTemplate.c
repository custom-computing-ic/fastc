typedef int count_t;

extern void output_ic(int* out, int res, int cond);
extern void output_i(int* out, int res);

void chain(count_t i, count_t j, count_t k) {}
int fselect(int cond, int val1, int val2) {return (cond ==0? val1 : val2);}
count_t count(int wrap, int inc) {return 1;}
count_t countChain(int wrap, int inc, count_t parent) {return 1;}
int scalar(){return 1;}

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
void kernel_KernelFunc(int *x, int *res,
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

  output_ic(res, func, cond);
}
