#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/kernel.hpp"
#define real double


//real* S1tbl, real* S2tbl, real* S3tbl are only useful for American options
real kernel(real* coef_arr, int* exe_params, real* grid, TOptionData* od, real* S1tbl, real* S2tbl, real* S3tbl, int type){
  real a_0_0_0  = coef_arr[0];
  real a_p1_0_0 = coef_arr[1];
  real a_m1_0_0 = coef_arr[2];
  real a_0_p1_0 = coef_arr[3];
  real a_0_m1_0 = coef_arr[4];
  real a_0_0_p1 = coef_arr[5];
  real a_0_0_m1 = coef_arr[6];
  real a_p1_p1_0= coef_arr[7];
  real a_p1_0_p1= coef_arr[8];
  real a_0_p1_p1= coef_arr[9];

  int N_t  = exe_params[0];
  int M_Z1 = exe_params[1];
  int M_Z2 = exe_params[2];
  int M_Z3 = exe_params[3];

  real* grid1 = (real*)malloc(M_Z1*M_Z2*M_Z3*sizeof(real));
  real* grid2 = (real*)malloc(M_Z1*M_Z2*M_Z3*sizeof(real));

  real* source = grid;
  real* target = grid1;

  int i,j,k,l;
  for(i=0;i<N_t;i++){
    real S1j, S2k, S3l; //not useful for type 1 option
#pragma omp parallel for
    for(l=1; l<M_Z3-1; l++){
      S3l = S3tbl[l];
      for(k=1; k<M_Z2-1; k++){
        S2k = S2tbl[k];
        for(j=1; j<M_Z1-1; j++){
          S1j = S1tbl[j];

          //node_j_k_l
          real node_0_0_0  = source[l*M_Z2*M_Z1+k*M_Z1+j];
          //node_j_k_l
          real node_p1_0_0 = source[l*M_Z2*M_Z1+k*M_Z1+(j+1)];
          real node_m1_0_0 = source[l*M_Z2*M_Z1+k*M_Z1+(j-1)];
          real node_0_p1_0 = source[l*M_Z2*M_Z1+(k+1)*M_Z1+j];
          real node_0_m1_0 = source[l*M_Z2*M_Z1+(k-1)*M_Z1+j];
          real node_0_0_p1 = source[(l+1)*M_Z2*M_Z1+k*M_Z1+j];
          real node_0_0_m1 = source[(l-1)*M_Z2*M_Z1+k*M_Z1+j];
          //node_j_k_l
          real node_p1_p1_0= source[l*M_Z2*M_Z1+(k+1)*M_Z1+(j+1)];
          real node_m1_p1_0= source[l*M_Z2*M_Z1+(k+1)*M_Z1+(j-1)];
          real node_p1_m1_0= source[l*M_Z2*M_Z1+(k-1)*M_Z1+(j+1)];
          real node_m1_m1_0= source[l*M_Z2*M_Z1+(k-1)*M_Z1+(j-1)];
          //node_j_k_l1
          real node_p1_0_p1= source[(l+1)*M_Z2*M_Z1+k*M_Z1+(j+1)];
          real node_m1_0_p1= source[(l+1)*M_Z2*M_Z1+k*M_Z1+(j-1)];
          real node_p1_0_m1= source[(l-1)*M_Z2*M_Z1+k*M_Z1+(j+1)];
          real node_m1_0_m1= source[(l-1)*M_Z2*M_Z1+k*M_Z1+(j-1)];
          //node_j_k_l
          real node_0_p1_p1= source[(l+1)*M_Z2*M_Z1+(k+1)*M_Z1+j];
          real node_0_m1_p1= source[(l+1)*M_Z2*M_Z1+(k-1)*M_Z1+j];
          real node_0_p1_m1= source[(l-1)*M_Z2*M_Z1+(k+1)*M_Z1+j];
          real node_0_m1_m1= source[(l-1)*M_Z2*M_Z1+(k-1)*M_Z1+j];


          real v_jkl= node_0_0_0  * a_0_0_0
            +
            ( node_p1_0_0 * a_p1_0_0
              + node_m1_0_0 * a_m1_0_0
              + node_0_p1_0 * a_0_p1_0
              + node_0_m1_0 * a_0_m1_0
              + node_0_0_p1 * a_0_0_p1
              + node_0_0_m1 * a_0_0_m1)
            +
            (node_p1_p1_0 -     node_m1_p1_0 - node_p1_m1_0 +   node_m1_m1_0) * a_p1_p1_0
            +
            (node_p1_0_p1 - node_m1_0_p1 - node_p1_0_m1 + node_m1_0_m1) * a_p1_0_p1
            +
            (node_0_p1_p1 - node_0_m1_p1 - node_0_p1_m1 + node_0_m1_m1) * a_0_p1_p1;

          real temp=0;
          if(type ==2){
            temp = pow(S1j*S2k*S3l, 0.333333);
            temp = od->K-temp;
            temp = temp>0? temp:0;
          }

          real payoff = v_jkl>temp? v_jkl:temp;
          target[l*M_Z2*M_Z1+k*M_Z1+j]=payoff;
          //printf("[%d][%f], ", l*M_Z2*M_Z1+k*M_Z1+j, grid[l*M_Z2*M_Z1+k*M_Z1+j]);
        }
        //printf("\n");
      }
      //printf("\n");
    }
    if(i!=0){
      real* tmpgrid = source;
      source = target;
      target = tmpgrid;
    }else{
      source = grid1;
      target=grid2;
    }
  }

  real res= target[(M_Z3/2)*M_Z2*M_Z1+(M_Z2/2)*M_Z1+(M_Z1/2)];
  delete grid1, grid2;
  return res;

}
