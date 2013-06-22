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
  int result = (*(int *)p1) - (*(int *)p2);
  if (result > 0) return 1;
  if (result < 0) return -1;
  return 0;
}

//int n = 134217728;
//int n = 8388608;
int n = 1048576;
const int array_width = 128;

struct timeval time3;
struct timeval time2;

double time_ms(struct timeval time1, struct timeval time0) {
	if (time1.tv_usec >= time0.tv_usec){
		return (time1.tv_sec - time0.tv_sec)+(time1.tv_usec - time0.tv_usec)*1e-6;
	} else{
		return (time1.tv_sec - time0.tv_sec-1)+(1e6+time1.tv_usec - time0.tv_usec)*1e-6;
	}

}

int main(int argc, char** argv) {

	int n = 1048576 * array_width;
	if (argc > 2)
		n = array_width * atoi(argv[2]);

	printf("n = %d", n);

  max_maxfile_t *maxfile = max_maxfile_init_PassThrough();
  max_device_handle_t *device = open_device(maxfile, argc, argv);

  int *x   = (int*) malloc( n * sizeof(int) );
  int *res = (int*) malloc( n * sizeof(int) );
  int *exp = (int*) malloc( n * sizeof(int) );

  srand(time(NULL));

  printf("Initializing data ... ");
  int i;
  for (i = 0; i < n; i++) {
    res[i] = 0;
    exp[i] = x[i] = rand(); //((float)rand() / (float)RAND_MAX);
  }
  printf("done!\n");

  gettimeofday(&time3, NULL);
	for (i = 0; i < n; i += array_width)
	  qsort(exp + i, array_width, sizeof(float), compare);
  gettimeofday(&time2, NULL);
  printf("CPU Sort: %.6f s\n", time_ms(time2, time3));

  printf("Initializing FPGA ... ");
  max_kernel_set_cycles(device, "PassThroughKernel", n / array_width, FPGA_A);
  max_redirect_sim_debug_output(device, "printf_output.txt" ) ;

  max_stream_handle_t *in_x  = max_get_pcie_stream(device, "in_array");
  max_stream_handle_t *out_y = max_get_pcie_stream(device, "out_array");
  printf("done!\n");

  printf("Processing data in FPGA ... ");
  gettimeofday(&time3, NULL);
	max_reset_device(device);
  max_queue_pcie_stream(device, in_x, x, n * sizeof(float), 1);
  max_queue_pcie_stream(device, out_y, res,n * sizeof(float), 1);
  max_sync_pcie_stream(device, out_y);
	gettimeofday(&time2, NULL);
  printf("done!\n");
	printf("FPGA Sort: %.6f s\n", time_ms(time2, time3));

  printf("Checking results...\n");
  for (i = 0; i < n; i++)
    if (res[i] != exp[i])
      printf("Error : res[%d] = %df! [Exp. %d] \n",  i, res[i], exp[i]);
  printf("done\n");

  printf("Shutting down ... ");
  max_close_device(device);
  max_destroy(maxfile);
  printf("done!\n");

//  for (i = 0; i < n; i++)
//    printf("%d ", exp[i]);

  free(x);
  free(res);

  return 0;
}
