/* max_util.c ---- Brittle 2011 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <MaxCompilerRT.h>
#include "max_util.h"

max_maxfile_t             *maxfile = NULL;
max_device_handle_t       *device = NULL;
struct max_memory_setting *ctx = NULL;


int MAX_Add(int start_address, int data_size, long long *address_stream){
  
 return 0;
}

int MAX_Conf_CustomAdd(char *stream_name, unsigned offset_bursts, int data_size, int Par, long long *address_stream, int write){
 
    int address_low;
    int burst_size;
    int address_high;
    int i=0;
    int count;
    long long address;
    int data_size_buf;
    int start_address;

    for (count=0;count<Par;count++)
    {  
      data_size_buf = data_size;
      start_address = offset_bursts;
      
      while(data_size_buf != 0)
      {
        burst_size    =  ((data_size_buf - 255) >= 0) ? 255             : data_size_buf;
        data_size_buf =  (burst_size ==255)       ? data_size_buf - 255 : 0;
        address_low   =  start_address;
        start_address += burst_size;
        printf ("burst_size:%d, start_address:%d\n",burst_size, address_low);
        
        address_high= (burst_size)  | 1 << 8 | 1 << 16; 
          
        address     = address_low |(long long) address_high<<32;
        if (write & burst_size != 255 & count==Par-1) address     = address | (long long) 1 <<63;
        address_stream[i] =address;
        i ++;
        //printf ("address:%lld\n",address);
      }
    }
    return 0;
//  char *group_name;
//  group_name = (char *)calloc(5+strlen(stream_name), sizeof(char));
//  strcpy(group_name, "cmd_");
//  strcat(group_name, stream_name);

//  max_memory_stream_set_start_address(ctx, stream_name, offset_bursts);
//  
//  return max_memory_commit_setting(device, ctx, FPGA_A); 

}

int MAX_CommitAdd(uint32_t *data_read,uint32_t *data_write, unsigned size_b, char *address_read,  char *address_write) {
    
  max_run(device,
        max_input(address_write, data_write, size_b),
        max_input(address_read,  data_read,  size_b),
        max_end());


  return 0;
}

int MAX_CommitAdd1(uint32_t *data,unsigned size_b, char *address) {
    
  max_run(device,
        max_input(address,  data,  size_b),
        max_end());


  return 0;
}
int MAX_Init(char *device_name) {
  int status = 0;

  printf("Opening FPGA card ... ");
  maxfile = max_maxfile_init_MyApp();
  max_set_maximum_runnable_timing_score(maxfile, -1);

  device = max_open_device(maxfile, device_name);
  max_set_terminate_on_error(device);
  printf("done!\n"); 

  printf("Initialising device ... ");
  ctx = default_max_memory_setting(maxfile);
  if (ctx == NULL) {
    printf("Error: creating memory setting!\n");
    status = 1;
  }
  printf("done!\n"); 

  return status;
}

int MAX_Conf_Mem1D_GBSL (
    char *stream_name,
    char *group_name,
    unsigned offset_bytes,
    unsigned bx_bytes,
    unsigned sx_bytes,
    unsigned lx_bytes) {

  int burst_length_bytes = 0;
  int offset_bursts = 0;
  int bx_bursts = 0;
  int sx_bursts = 0;
  int lx_bursts = 0;
 
  burst_length_bytes = max_group_burst_length(maxfile, group_name);

  if ( (offset_bytes % burst_length_bytes) ||
       (bx_bytes % burst_length_bytes) ||
       (sx_bytes % burst_length_bytes) ||
       (lx_bytes % burst_length_bytes) ) {
    printf("Error: Stream %s does not align with burst size %d!\n",
        stream_name, burst_length_bytes);
    goto err_exit;
  }

  offset_bursts = offset_bytes/burst_length_bytes;
  bx_bursts = bx_bytes/burst_length_bytes;
  sx_bursts = sx_bytes/burst_length_bytes;
  lx_bursts = lx_bytes/burst_length_bytes;

  if (max_memory_stream_set_access_pattern_linear1d(ctx, group_name,
        bx_bursts, sx_bursts, lx_bursts)) {
    printf("Error: Cannot set %s to 1D pattern!\n", stream_name);
    goto err_exit;
  }

  if (max_memory_stream_set_start_address(ctx, stream_name, offset_bursts)) {
    printf("Error: Cannot start %s@%d(bursts)!\n", stream_name, offset_bursts);
    goto err_exit;
  }

  return max_memory_commit_setting(device, ctx, FPGA_A);
  
  return 0;
err_exit:
  return -1;
}

int MAX_Conf_Mem1D_G(
    char *stream_name,
    char *group_name,
    unsigned offset_bytes,
    unsigned data_length_bytes,
    unsigned iterations) {

  return MAX_Conf_Mem1D_GBSL(
      stream_name, group_name, offset_bytes,
      iterations*data_length_bytes, 0, data_length_bytes);
}

int MAX_Conf_Mem1D(
    char *stream_name,
    unsigned offset_bytes,
    unsigned data_length_bytes,
    unsigned iterations) {

  char *group_name;
  int status = 0;

  group_name = (char *)calloc(5+strlen(stream_name), sizeof(char));
  strcpy(group_name, "cmd_");
  strcat(group_name, stream_name);
  status = MAX_Conf_Mem1D_G(
      stream_name, group_name, offset_bytes, data_length_bytes, iterations);

  free(group_name);
  return status;
}


int MAX_Enable_Mem(char *stream_name) {

  if (max_memory_stream_set_enable(ctx, stream_name, 1)) {
    printf("Error: Cannot enable stream %s!\n", stream_name);
    return -1;
  } else
    return max_memory_commit_setting(device, ctx, FPGA_A);

}

int MAX_Disable_Mem(char *stream_name) {

  if (max_memory_stream_set_enable(ctx, stream_name, 0)) {
    printf("Error: Cannot disable stream %s!\n", stream_name);
    return -1;
  } else
    return max_memory_commit_setting(device, ctx, FPGA_A);

}

int MAX_PCI2Mem(uint32_t *data, unsigned size_b, char *stream_pci, char *stream_mem) {
  int status = 0;

  status =  MAX_Enable_Mem(stream_mem);
  status |= MAX_Set_Int(stream_mem);
  if (!status) {
    max_run(device,
        max_input(stream_pci, data, size_b),
        max_end());
  //interrupt is need to address the latency between PCIe and DRAM
  max_wait_for_interrupt(device, FPGA_A);
  status |= MAX_Disable_Mem(stream_mem);
  }

  if (status)
    printf("Error: from stream %s to stream %s!\n", stream_pci, stream_mem);

  return status;}

int MAX_Mem2PCI(void *data, int size_b, char *stream_pci, char *stream_mem) {
  int status = 0;

  status = MAX_Enable_Mem(stream_mem);
  max_run(device,
      max_input(stream_pci, data, size_b),
      max_end());
  status |= MAX_Disable_Mem(stream_mem);

  if (status)
    printf("Error: from stream %s to stream %s!\n", stream_mem, stream_pci);

  return status;
}

void MAX_Reset(void) {
  max_reset_device(device);
}

int MAX_Set_Int(char *stream_name) {
  max_memory_stream_interrupt_none(ctx);
  max_memory_stream_interrupt_any(ctx);
//if (max_memory_stream_interrupt_on(ctx, stream_name, NULL)) {
//printf("Error: Cannot set interrupt on %s!\n", stream_name);
//goto err_exit;
//}
  return max_memory_commit_setting(device, ctx, FPGA_A);
err_exit:
  return -1;
}

void MAX_Wait_Int(void) {
    max_wait_for_interrupt(device, FPGA_A);
    //max_memory_stream_interrupt_any(ctx);
}

void MAX_Set_Cycles(char *kernel_name, int size) {
  max_kernel_set_cycles(device, kernel_name, size, FPGA_A);
}

void MAX_Set_FScalar(char *scalar_symbol, float value) {
  max_set_scalar_input_f(device, scalar_symbol, value, FPGA_A);
}

void MAX_Finish(void) {

  printf("Cleaning up resources ... ");
  delete_max_memory_setting(ctx);
  printf("done!\n");

  printf("Shutting down ... ");
  max_close_device(device);
  max_destroy(maxfile);
  printf("done!\n");
}
