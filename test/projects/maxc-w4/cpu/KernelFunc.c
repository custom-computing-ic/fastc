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


int a1 = 1, a2 = 1, a3 = 1;
int n1 = 21, n2 = 21, n3 = 21;
int nx = 64, ny = 64, nz = 64;
double c = 2.0;

void set_scalars(max_device_handle_t* device) {
  max_set_scalar_input_f(device, "KernelFunc.c", 2.0, FPGA_A);

  max_set_scalar_input(device, "KernelFunc.a1", a1, FPGA_A);
  max_set_scalar_input(device, "KernelFunc.a2", a2, FPGA_A);
  max_set_scalar_input(device, "KernelFunc.a3", a3, FPGA_A);

  max_set_scalar_input(device, "KernelFunc.n1", n1, FPGA_A);
  max_set_scalar_input(device, "KernelFunc.n2", n2, FPGA_A);
  max_set_scalar_input(device, "KernelFunc.n3", n3, FPGA_A);

  max_set_scalar_input(device, "KernelFunc.nx", nx, FPGA_A);
  max_set_scalar_input(device, "KernelFunc.ny", ny, FPGA_A);
  max_set_scalar_input(device, "KernelFunc.nz", nz, FPGA_A);
}

int main(int argc, char** argv) {

  max_maxfile_t *maxfile = max_maxfile_init_KernelFunc();
  max_device_handle_t *device = open_device(maxfile, argc, argv);

  float *x   = (float *) malloc(nx * ny * nz * sizeof(float) );
  float *res = (float *) malloc((n1 - a1) * sizeof(float) );

  printf("Initializing data ... ");
  int i, j, k;
  for (i=0; i < n1 - a1; i++) res[i] = 0;

  for (i = 0; i < nz; i++)
    for (j = 0; j < ny; j++)
      for (k =0; k < nx; k++)
       x[i * nx * ny + j * nx + k] = i + j + k;
  printf("done!\n");

  printf("Initializing FPGA ... ");
  max_kernel_set_cycles(device, "KernelFunc", nx * ny * nz, FPGA_A);
  max_redirect_sim_debug_output(device, "printf_output.txt" ) ;

  set_scalars(device);

  max_stream_handle_t *x_stream = max_get_pcie_stream(device, "x");
  max_stream_handle_t *res_stream = max_get_pcie_stream(device, "res");
  printf("done!\n");

  printf("Processing data in FPGA ... ");
  max_reset_device(device);
  max_queue_pcie_stream(device, x_stream, x, nx * ny * nz * sizeof(float), 1);
  max_queue_pcie_stream(device, res_stream, res, (n1 - a1) * sizeof(float), 1);

  max_sync_pcie_stream(device, res_stream);
  printf("done!\n");

  printf("Checking results...\n");
  float * exp = (float *)malloc( nx * sizeof(float));
  for (i =0; i < nx; i++) exp[i] = 0;

  for (i = a1; i < n1; i++)
    for (j = a2; j < n2; j++)
      for (k = a3; k < n3; k++)
          exp[i] = ( exp[i] == 0 ? 
            c * ( x[i * nx * ny + j * nx + k] + 
                  x[i * nx * ny + j * nx + k  - 1]  + 
                  x[i * nx * ny + j * nx + k  + 1]  + 
                  x[i * nx * ny + (j - 1) * nx + k] + 
                  x[i * nx * ny + (j + 1) * nx + k] + 
                  x[(i + 1) * nx * ny + j * nx + k] +
                  x[(i - 1) * nx * ny + j * nx + k]) : exp[i] ); 

  for (i = a1; i < n1; i++)
    if (res[i - a1] != exp[i])
      printf("Error : %f at %d! [Expected %f] \n",  res[i], i, exp[i]);
  printf("done\n");

  printf("Shutting down ... ");
  max_close_device(device);
  max_destroy(maxfile);
  printf("done!\n");

  free(x);
  free(res);

  return 0;
}

