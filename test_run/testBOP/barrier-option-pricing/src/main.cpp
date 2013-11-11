#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "../inc/kernel.hpp"
#define real double

struct timeval fpga_time0;
struct timeval fpga_time1;
struct timeval fpga_time2;


int main()
{
  TOptionData* od =new TOptionData();
  od->S1=80;
  od->S2=81;
  od->S3=82;
  od->K =70;
  od->R =0.05;
  od->T =1.0;
  od->V1=0.3;
  od->V2=0.4;
  od->V3=0.5;
  od->Corr1_2=0.1;
  od->Corr1_3=0.2;
  od->Corr2_3=0.15;
  //if S3>40, payoff = max(K-(S1+S2+S3)/3,0)
  //else payoff = max(K - pow(S1*S2*S3, (double)1/3),0)
  od->barrier = 40;

  //num. timesteps
  int N_t  = 100;
  int M_Z1 = 512;
  int M_Z2 = 1024;
  int M_Z3 = 1024;

  int exe_params[4] = {N_t,M_Z1,M_Z2,M_Z3};

  real* grid = (real*)malloc(M_Z1*M_Z2*M_Z3*sizeof(real));
  real* S1tbl = (real*)malloc(M_Z1*sizeof(real));
  real* S2tbl = (real*)malloc(M_Z2*sizeof(real));
  real* S3tbl = (real*)malloc(M_Z3*sizeof(real));
  //cover a 10 sigma range at T for each asset
  real spanS1 = 5*od->V1*sqrt(od->T)*od->S1;
  real spanS2 = 5*od->V2*sqrt(od->T)*od->S2;
  real spanS3 = 5*od->V3*sqrt(od->T)*od->S3;
  //change of variable: Z=lnS
  const real eps = 0.0001;
  real startZ1 = od->S1-spanS1>0? od->S1-spanS1 : eps;
  startZ1 = log(startZ1);
  real startZ2 = od->S2-spanS2>0? od->S2-spanS2 : eps;
  startZ2 = log(startZ2);
  real startZ3 = od->S3-spanS3>0? od->S3-spanS3 : eps;
  startZ1 = log(startZ3);

  real endZ1 = log(od->S1+spanS1);
  real endZ2 = log(od->S2+spanS2);
  real endZ3 = log(od->S3+spanS3);

  real spanZ1 = endZ1-startZ1;
  real spanZ2 = endZ2-startZ2;
  real spanZ3 = endZ3-startZ3;

  real dZ1 = spanZ1/M_Z1;
  real dZ2 = spanZ2/M_Z2;
  real dZ3 = spanZ3/M_Z3;
  printf("span1=%f, span2=%f, span3=%f\n",spanS1, spanS2, spanS3);
  int i,j,k,l;
  real Z = startZ1;
  for(i=0;i<M_Z1;i++){
    S1tbl[i] = exp(Z);
    //printf("real S1 = %f\n", S1tbl[i]);
    Z += dZ1;
  }
  Z = startZ2;
  for(i=0;i<M_Z2;i++){
    S2tbl[i] = exp(Z);
    //printf("real S1 = %f\n", S1tbl[i]);
    Z += dZ2;
  }
  Z = startZ3;
  for(i=0;i<M_Z3;i++){
    S3tbl[i] = exp(Z);
    //printf("real S1 = %f\n", S1tbl[i]);
    Z += dZ3;
  }
  //assume j,k,l is the current node being updated
  //and is named as g_0_0_0, then j+1,k,l is g_p1_0_0
  //j-1,k,l is g_m1_0_0  etc.

  real S1j, S2k, S3l;
  for(l=0; l<M_Z3; l++ ){
    S3l = S3tbl[l];
    for(k=0; k<M_Z2; k++){
      S2k = S2tbl[k];
      for(j=0; j<M_Z1; j++){
        S1j = S1tbl[j];
        real temp1, temp2;
        temp1 = (S1j+S2k+S3l)/3;
        //temp1 = pow(S1j*S2k*S3l, 0.333333);
        //if(S3l>od->barrier){
        //      temp2 = (S1j+S2k+S3l)/3;
        //      temp1 = temp2<temp1? temp2: temp1;
        //}
        real payoff = od->K-temp1;
        payoff = payoff>0? payoff:0;
        grid[l*M_Z2*M_Z1+k*M_Z1+j]=payoff;
        //printf("[%d][%f], ", l*M_Z2*M_Z1+k*M_Z1+j, grid[l*M_Z2*M_Z1+k*M_Z1+j]);
      }
      //printf("\n");
    }
    //printf("\n");
  }

  //now calculate coefficients
  real dt = od->T/N_t;
  real tmp1 = (real)1/(od->R*dt+1);
  real sig1_2 =od->V1*od->V1;
  real sig2_2 =od->V2*od->V2;
  real sig3_2 =od->V3*od->V3;
  real dZ1_2 = dZ1*dZ1;
  real dZ2_2 = dZ2*dZ2;
  real dZ3_2 = dZ3*dZ3;

  real tmp2 = dt/dZ1_2*sig1_2;
  real tmp3 = dt/dZ2_2*sig2_2;
  real tmp4 = dt/dZ3_2*sig3_2;

  real a_0_0_0 = 1-tmp2-tmp3-tmp4;
  //a_0_0_0 *=tmp1;
  real tmp5 = dt/2/dZ1*(od->R - sig1_2/2);
  real tmp6 = dt/2/dZ2*(od->R - sig2_2/2);
  real tmp7 = dt/2/dZ3*(od->R - sig3_2/2);
  real a_p1_0_0 = tmp5+tmp2/2;
  real a_m1_0_0 = -tmp5+tmp2/2;
  real a_0_p1_0 = tmp6+tmp3/2;
  real a_0_m1_0 = -tmp6+tmp3/2;
  real a_0_0_p1 = tmp7+tmp4/2;
  real a_0_0_m1 = -tmp7+tmp4/2;
  real a_p1_p1_0 = dt*od->Corr1_2*od->V1*od->V2;
  a_p1_p1_0 = a_p1_p1_0/8/dZ1/dZ2;
  real a_p1_0_p1= dt*od->Corr1_3*od->V1*od->V3;
  a_p1_0_p1 = a_p1_0_p1/8/dZ1/dZ3;
  real a_0_p1_p1= dt*od->Corr2_3*od->V2*od->V3;
  a_0_p1_p1= a_0_p1_p1/8/dZ2/dZ3;

  real coef_arr[10];
  coef_arr[0] = a_0_0_0*tmp1;
  coef_arr[1] = a_p1_0_0*tmp1;
  coef_arr[2] = a_m1_0_0*tmp1;
  coef_arr[3] = a_0_p1_0*tmp1;
  coef_arr[4] = a_0_m1_0*tmp1;
  coef_arr[5] = a_0_0_p1*tmp1;
  coef_arr[6] = a_0_0_m1*tmp1;
  coef_arr[7] = a_p1_p1_0*tmp1;
  coef_arr[8] = a_p1_0_p1*tmp1;
  coef_arr[9] = a_0_p1_p1*tmp1;
  //type 1 is European, type 2 is American

  gettimeofday(&fpga_time0,NULL);

  int type =1;
  real result1 = kernel(coef_arr, exe_params, grid, od, S1tbl, S2tbl, S3tbl,type);
  printf("option1 payoff = %f\n",result1);


  gettimeofday(&fpga_time1,NULL);

  printf("Option Type 1 Computing Time = %f\n", (fpga_time1.tv_sec  - fpga_time0.tv_sec)
         +(fpga_time1.tv_usec - fpga_time0.tv_usec)*1e-6);


  type =2;
  //real result2 = kernel(coef_arr, exe_params, grid, od, S1tbl, S2tbl, S3tbl,type);
  //printf("option2 payoff = %f\n",result2);

  gettimeofday(&fpga_time2,NULL);
  printf("Option Type 2 Computing Time = %f\n", (fpga_time2.tv_sec  - fpga_time1.tv_sec)
         +(fpga_time2.tv_usec - fpga_time1.tv_usec)*1e-6);

  //real total = result1+result2;
  //printf("the payoff of the product = %f", total);

  delete od;
  delete S1tbl;
  delete S2tbl;
  delete S3tbl;
  delete grid;
}
