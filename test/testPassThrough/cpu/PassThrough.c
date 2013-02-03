#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#include <MaxCompilerRT.h>

max_device_handle_t* open_device(max_maxfile_t *maxfile, int argc, char **argv) {
  printf("Opening FPGA card ... ");
  max_device_handle_t *device = NULL;
  if (argc == 2)
    device = max_open_device(maxfile, argv[1]);
  else
    device = max_open_device(maxfile, "/dev/maxeler0");
  max_set_terminate_on_error(device);
  printf("done!\n");
  return device;
}

int n = 1024;

int main(int argc, char** argv) {

  max_maxfile_t *maxfile = max_maxfile_init_PassThrough();
  max_device_handle_t *device = open_device(maxfile, argc, argv);

  float *x   = (float *) malloc( n * sizeof(float) );
  float *res = (float *) malloc( n * sizeof(float) );

  printf("Initializing data ... ");
  int i;
  for (i = 0; i < n; i++) {
    res[i]=0;
    x[i] = 2 * i;
  }
  printf("done!\n");

  printf("Initializing FPGA ... ");
  max_kernel_set_cycles(device, "PassThroughKernel", n, FPGA_A);
  max_redirect_sim_debug_output(device, "printf_output.txt" ) ;

  max_stream_handle_t *in_x  = max_get_pcie_stream(device, "x");
  max_stream_handle_t *out_y = max_get_pcie_stream(device, "y");
  printf("done!\n");

  printf("Processing data in FPGA ... ");
  max_reset_device(device);
  max_queue_pcie_stream(device, in_x, x, n * sizeof(float), 1);
  max_queue_pcie_stream(device, out_y, res, n * sizeof(float), 1);
  max_sync_pcie_stream(device, out_y);
  printf("done!\n");

  printf("Checking results...\n");
  for (i = 0; i < n; i++)
    if (res[i] != 2 *i)
      printf("Error : %f at %d! [Expected %f] \n",  res[i], i, 2 * i);
  printf("done\n");

  printf("Shutting down ... ");
  max_close_device(device);
  max_destroy(maxfile);
  printf("done!\n");

  free(x);
  free(res);

  return 0;
}

