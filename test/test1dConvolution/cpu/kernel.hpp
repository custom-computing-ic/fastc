#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define real float
using namespace std;

typedef struct{
    real S1;
    real S2;
	  real S3;
    real K;
    real T;
    real R;
    real V1;
    real V2;
	  real V3;
    real Corr1_2;
	  real Corr1_3;
	  real Corr2_3;
	  real barrier;
}TOptionData;

real kernel(real* coef_arr, int* exe_params, real* grid, TOptionData* od, real* S1tbl, real* S2tbl, real* S3tbl, int type);

real kernel_fpga(real* coef_arr, int* exe_params, real* grid, TOptionData* od, real* S1tbl, real* S2tbl, real* S3tbl, int type);
