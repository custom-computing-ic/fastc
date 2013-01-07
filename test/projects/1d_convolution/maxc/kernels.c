typedef int count_t;

extern void output_ic(int* out, int res, int cond);
extern void output_i(int* out, int res);

void chain(count_t i, count_t j, count_t k) {}
int fselect(int cond, int val1, int val2) {return (cond ==0? val1 : val2);}
count_t count(int wrap, int inc) {return 1;}
count_t countChain(int wrap, int inc, count_t parent) {return 1;}
int scalar(){return 1;}
/*
#pragma in x CPU kernel_func
#pragma out res CPU kernel_func
#pragma scalar in c float8_24 kernel_func
#pragma scalar in a1 uint32 kernel_func
#pragma scalar in a2 uint32 kernel_func
#pragma scalar in a3 uint32 kernel_func
#pragma scalar in n1 uint32 kernel_func
#pragma scalar in n2 uint32 kernel_func
#pragma scalar in n3 uint32 kernel_func
#pragma scalar in nx uint32 kernel_func
#pragma scalar in ny uint32 kernel_func
#pragma scalar in nz uint32 kernel_func
void kernel_func(
	 int *x, int *res,
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
}*/

#pragma in x CPU kernel_PassThrough
#pragma out y CPU kernel_PassThrough
void kernel_PassThrough(int *x, int *y) {
	output_i(y, x[0]);
}

#define burst_inc 1
#pragma scalar in iniBursts uint32 kernel_Cmdread
#pragma scalar in totalBursts uint32 kernel_Cmdread
#pragma scalar in wordsPerBurst uint32 kernel_Cmdread
#pragma scalar in Enable uint32 kernel_Cmdread
void kernel_Cmdread(
		int iniBursts, int totalBursts, int wordsPerBurst,
		int Enable) {

	int wordCount  = count(32, 1);
	int burstCount = count(32, burst_inc);

	int Control = (wordCount == 0) & Enable;

	DRAMoutput("dram_read",
			Control,
			burstCount + iniBursts,
			burst_inc,
			1,
			0,
			0);
}

