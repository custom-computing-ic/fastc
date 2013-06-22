#include <time.h>
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

static int
compare(const void *p1, const void *p2) {
  float result = (*(float *)p1) - (*(float *)p2);
  if (result > 0) return 1;
  if (result < 0) return -1;
  return 0;
}

int n = 128;

int main(int argc, char** argv) {

  max_maxfile_t *maxfile = max_maxfile_init_PassThrough();
  max_device_handle_t *device = open_device(maxfile, argc, argv);

  float *x   = (float *) malloc( n * sizeof(float) );
  float *res = (float *) malloc( n * sizeof(float) );
  float *exp = (float *) malloc( n * sizeof(float) );

  srand(time(NULL));

  printf("Initializing data ... ");
  int i;
  for (i = 0; i < n; i++) {
    res[i] = 0;
    exp[i] = x[i] = ((float)rand() / (float)RAND_MAX);
  }
  qsort(exp, n, sizeof(float), compare);
  printf("done!\n");

  printf("Initializing FPGA ... ");
  max_kernel_set_cycles(device, "PassThroughKernel", 1, FPGA_A);
  max_redirect_sim_debug_output(device, "printf_output.txt" ) ;

  max_stream_handle_t *in_x  = max_get_pcie_stream(device, "in_array");
  max_stream_handle_t *out_y = max_get_pcie_stream(device, "out_array");
  printf("done!\n");

  printf("Processing data in FPGA ... ");
  max_reset_device(device);
  max_queue_pcie_stream(device, in_x, x, n * sizeof(float), 1);
  max_queue_pcie_stream(device, out_y, res, n * sizeof(float), 1);
  max_sync_pcie_stream(device, out_y);
  printf("done!\n");

  printf("Checking results...\n");
  for (i = 0; i < n; i++)
    if (res[i] != exp[i])
      printf("Error : res[%d] = %.3f! [Exp. %.3f] \n",  i, res[i], exp[i]);
  printf("done\n");

  printf("Shutting down ... ");
  max_close_device(device);
  max_destroy(maxfile);
  printf("done!\n");

  for (i = 0; i < n; i++)
    printf("%f ", exp[i]);

  free(x);
  free(res);

  return 0;
}
