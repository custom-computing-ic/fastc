#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kernel.hpp"
#include "fpga.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

extern char *device_name;

//real* S1tbl, real* S2tbl, real* S3tbl are only useful for American options
real kernel(real* coef_arr, int* exe_params, real* grid, TOptionData* od, real* S1tbl, real* S2tbl, real* S3tbl, int type){
  real a_0_0_0  = coef_arr[0];
  real a_p1_0_0 = coef_arr[1];
  real a_m1_0_0 = coef_arr[2];

  int N_t  = exe_params[0];
  int M_Z1 = exe_params[1];

  real* grid1 = (real*)malloc(M_Z1*sizeof(real));
  real* grid2 = (real*)malloc(M_Z1*sizeof(real));

  real* source = grid;
  real* target = grid1;

  int i,j,k,l;
  for(i=0;i<N_t;i++){
    for(j=1; j<M_Z1-1; j++){
      //node_j_k_l
      real node_0_0_0  = source[j];

      //node_j_k_l
      real node_p1_0_0 = source[(j+1)];
      real node_m1_0_0 = source[(j-1)];

      real v_jkl= node_0_0_0  * a_0_0_0
        + node_p1_0_0 * a_p1_0_0
        + node_m1_0_0 * a_m1_0_0;
      real payoff = v_jkl;
      target[j]=payoff;
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
g
  real res= target[(M_Z1/2)];
  delete grid1, grid2;
  return res;

}

real kernel_fpga(real* coef_arr, int* exe_params, real* grid, TOptionData* od, real* S1tbl, real* S2tbl, real* S3tbl, int type){
  real a_0_0_0  = coef_arr[0];
  real a_p1_0_0 = coef_arr[1];
  real a_m1_0_0 = coef_arr[2];

  int N_t  = exe_params[0];
  int M_Z1 = exe_params[1];


  //fpga initialisation
  cout<<"Initialing..."<<endl;
  maxfile = max_maxfile_init_Convolution1d();
  max_set_maximum_runnable_timing_score(maxfile, -1);
  device  = max_open_device(maxfile, device_name);
  max_set_terminate_on_error(device);
  ctx = default_max_memory_setting(maxfile);
  max_set_interrupt_timeout(device, 100000);

  //paramter setting
  cout<<"Parameter Setting..."<<endl;
  int Inter         = N_t;
  int ORDER         = 1;
  int Burst_inc     = 1;
  int DATA_SIZE     = M_Z1;
  int BYTE_SIZE     = M_Z1* sizeof(float);
  int Par           = 1;
  int BURST_SIZE    = 384;
  int total_bursts  = BYTE_SIZE / BURST_SIZE;
  int stream_bursts = M_Z1 * sizeof(float) / BURST_SIZE;
  int stream_length = Par * sizeof(float);
  int stream_cycles = BURST_SIZE / stream_length;
  int PCI_width     = 8;
  int cycle_PCI     = BURST_SIZE / (2 * PCI_width);

  printf("total bursts:%d\n",total_bursts);

  //kernel cycle configuration
  max_kernel_set_cycles(device, "MyApp",       Inter * DATA_SIZE/ Par, FPGA_A);
  max_kernel_set_cycles(device, "Cmdread0",     Inter * DATA_SIZE/ Par, FPGA_A);
  max_kernel_set_cycles(device, "Cmdwrite0",    Inter * DATA_SIZE/ Par, FPGA_A);

  max_set_scalar_input(device,"Cmdread0.Enable", 0,    FPGA_A);
  max_set_scalar_input(device,"Cmdwrite0.Enable",0,    FPGA_A);

  //constant coefficients
  max_set_scalar_input_f(device,"MyApp.c_0_0_0", coef_arr[0], FPGA_A);
  max_set_scalar_input_f(device,"MyApp.c_p_0_0", coef_arr[1], FPGA_A);
  max_set_scalar_input_f(device,"MyApp.c_n_0_0", coef_arr[2], FPGA_A);

  //problem dimension
  max_set_scalar_input(device,"MyApp.n1",   M_Z1,    FPGA_A);
  max_set_scalar_input(device,"MyApp.ORDER",ORDER,   FPGA_A);

  //memory stream configurations
  max_set_scalar_input(device,"Cmdread0.iniBursts",      0,                            FPGA_A);
  max_set_scalar_input(device,"Cmdread0.totalBursts",    stream_bursts,                FPGA_A);
  max_set_scalar_input(device,"Cmdread0.wordsPerBurst",  stream_cycles * Burst_inc,    FPGA_A);

  max_set_scalar_input(device,"Cmdwrite0.iniBursts",     0,                            FPGA_A);
  max_set_scalar_input(device,"Cmdwrite0.iterations",    Inter,                        FPGA_A);
  max_set_scalar_input(device,"Cmdwrite0.totalBursts",   stream_bursts,                FPGA_A);
  max_set_scalar_input(device,"Cmdwrite0.wordsPerBurst", stream_cycles * Burst_inc,    FPGA_A);


  real* grid1 = (real*)malloc(M_Z1 * sizeof(real));
  real* grid2 = (real*)malloc(M_Z1 * sizeof(real));
  real* comb  = (real*)malloc(M_Z1 * sizeof(real));
  real* result= (real*)malloc(M_Z1 * sizeof(real));

  real* source = grid;
  real* target = grid1;

  //transfer data to FPGA
  max_kernel_set_cycles(device,"Cmdhostwrite",         total_bursts * cycle_PCI,  FPGA_A);
  max_set_scalar_input( device,"Cmdhostwrite.iterations",   1,                    FPGA_A);
  max_set_scalar_input( device,"Cmdhostwrite.totalBursts",  total_bursts,         FPGA_A);
  max_set_scalar_input( device,"Cmdhostwrite.wordsPerBurst",cycle_PCI * Burst_inc,FPGA_A);

  cout << "Scalar setup complete." << endl;

  int count =0;
  for(int j=0; j<M_Z1; j++)
    {
      comb[count] = source[j];
      count++;
    }

  cout << "Writing data to memory..." << endl;
  max_set_scalar_input( device,"Cmdhostwrite.Enable",       1,            FPGA_A);
  max_memory_stream_interrupt_on(ctx, "mgr2dram", NULL);
  max_run(device, max_input("host2mgr", comb, BYTE_SIZE), max_end());
  max_wait_for_interrupt(device, FPGA_A);
  max_set_scalar_input(device,"Cmdhostwrite.Enable",        0,            FPGA_A);
  cout << "Done" << endl;

  cout << "Start FPGA execution..." << endl;
  //FPGA execution
  max_set_scalar_input(device,"Cmdread0.Enable",         1,               FPGA_A);
  max_set_scalar_input(device,"Cmdwrite0.Enable",        1,               FPGA_A);

  max_memory_stream_interrupt_on(ctx, "knl2dram0", NULL);
  max_reset_device(device);
  max_wait_for_interrupt(device, FPGA_A);

  max_set_scalar_input(device,"Cmdread0.Enable",         0,               FPGA_A);
  max_set_scalar_input(device,"Cmdwrite0.Enable",        0,               FPGA_A);
  printf("kernel finished\n");

  //transfer data to CPU
  max_kernel_set_cycles(device,"Cmdhostread",              total_bursts * cycle_PCI, FPGA_A);
  max_set_scalar_input( device,"Cmdhostread.iniBursts",    0,                        FPGA_A);
  max_set_scalar_input( device,"Cmdhostread.totalBursts",  total_bursts,             FPGA_A);
  max_set_scalar_input( device,"Cmdhostread.wordsPerBurst",cycle_PCI * Burst_inc,    FPGA_A);

  max_set_scalar_input( device,"Cmdhostread.Enable",       1,             FPGA_A);
  max_run(device, max_output("mgr2host", comb, BYTE_SIZE), max_end());
  max_set_scalar_input(device,"Cmdhostread.Enable",        0,             FPGA_A);

  for(int j=0; j<M_Z1; j++)
    result[j] = comb[j];

  for(int j=0; j<M_Z1; j++)
    target[j] = source[j];

  for(int i=0;i<N_t;i++){
    for(int j=1; j<M_Z1-1; j++){
      //node_j_k_l
      real node_0_0_0  = source[j];
      //node_j_k_l
      real node_p1_0_0 = source[(j+1)];
      real node_m1_0_0 = source[(j-1)];

      real v_jkl= node_0_0_0  * a_0_0_0
        + node_p1_0_0 * a_p1_0_0
        + node_m1_0_0 * a_m1_0_0;
      real payoff = v_jkl;
      target[j]=payoff;
    }
    //   if(i!=0){
    real* tmpgrid = source;
    source = target;
    target = tmpgrid;
    //  }else{
    //          source = grid1;
    //          target=grid2;
    //  }
  }

  for(int j=1; j<M_Z1-1; j++)
    if(result[j] != source[j])
      {
        cout<<"Error: j: "<<j<<endl;
        cout<<"should:"<<setprecision(24)<<source[j]
            <<setprecision(24) <<"get"<<result[j];
      }
  cout<<endl;

  real res= source[(M_Z1/2)];
  delete grid1, grid2;
  return res;

}
