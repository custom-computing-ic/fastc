/*
* Main file for steering RTM execution
* Xinyu Niu, 14/11/2011
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <MaxCompilerRT.h>
   
//turn off the random seed for debug purposes
//#define FIXED_SEED

#include "cpu_constructors.h"
#include "cpu_simulation.h"
extern max_maxfile_t       *maxfile;
extern max_device_handle_t *device;

struct timeval time3;
struct timeval time2;
struct timeval time1;
struct timeval time0;
char *device_name;  
// S is the source
// G is the receiver
// w is the frequency
// s is the shot index
// h is the offset
// Image(h,x,z)=Sum(s,w){S(x-h,z,w,s).G(x+h,z,w,s)}

int main(int argc, char **argv){

  //string *dev = new string("/dev/maxeler0");
//  device_name = (argc==2 ? argv[1] : dev->c_str());  

  device_name = argv[1];

  //app variables
  char *earth_model_file=NULL;
	char receiver_file[64];
	char out_file[64];

  //initialisation FPGA
//printf("Opening FPGA card ... ");
//maxfile = max_maxfile_init_MyApp();
//device = max_open_device(maxfile, device_name);
//max_set_terminate_on_error(device);
//printf("done!\n");
  //MAX_Init(device_name);
	//// 1 Initializations

#ifdef FIXED_SEED
	irand=20;
#else
	struct timeval t;
	gettimeofday(&t,NULL);
	irand=t.tv_sec;
#endif

	if(argc!=2){
		fprintf(stderr,"Error: invalid arguments in command line: please provide the parameter file path!\n");
		exit(-1);
	}
  
  char *params_file = "./tests/rtm_parameters_small.txt";
	earth_model_file=create_params(params_file);

	//folder_path is initialized in create_params()
	strcpy(out_file,folder_path);
	strcat(out_file,"/image.out");

	sprintf(receiver_file,"./tmp/receiver_name_%.2f_%.4f_%d_%d_%d_%d_%d_%.2f",d1,dt,nf,jt,isx,isy,isz,fpeak);

	//// 2 Constructors
  fprintf(stderr,"CPU version with random boundaries\n");

	// Create Wavefields and Image
	create_fields();
	// Create data array (receiver signal)
	create_data();

	//Condition to check if we need to preprocess the receiver
	//   or if it is already available in a file from an earlier execution
	if(!rerun_receiver)rerun_receiver=get_receiver_from_file(receiver_file);

	// Load or create earth model
	if (earth_model_file) read_earth_model(earth_model_file);
	else                  create_vel();

	// Construct sponge region
	TOTAL_SPONGE_WIDTH=SPONGE_WIDTH;
	construct_bound();

	// Create Random Boundary Around Computational Domain
	random_bound_dvv();
	square_earth_model();
    
	//// 3 Simulation
	// Stencil
	construct_stencil();

	gettimeofday(&time0,NULL);

	if(rerun_receiver){
		//Advance Source wavefield to maxtime with sponge to gather receiver data
		prop_source(1);
		//we must clean the source and the receivers in order to use the same arrays for the next steps
		clean_source();
		clean_receiver_field();

		//Eliminate the source signal from the recorded receiver field
		differentiate_mute_data(receiver_file);
		fprintf(stderr,"FINISHED RECORDING RECEIVER DATA\n");
	}
	gettimeofday(&time1,NULL);

	//No damping - reverse algorithm
	//Propagate to maxtime with random boundaries
	prop_source(0);
	fprintf(stderr,"FINISHED PROPAGATING SOURCE \n");

	gettimeofday(&time1,NULL);
	gettimeofday(&time2,NULL);

	//Migrate - backward propagation
	migrate_shot();
	fprintf(stderr,"FINISHED MIGRATING \n");

	//processing times
	gettimeofday(&time3,NULL);

	//// 4 Image
	//store final image
	dump_image_to_file(out_file);

	//// 5 Print out execution times and cleanup
	double total_time;

	if (time1.tv_usec >= time0.tv_usec){
	        printf("Receiver forwarding = %f\n", (time1.tv_sec - time0.tv_sec)+(time1.tv_usec - time0.tv_usec)*1e-6);
	        total_time=(time1.tv_sec - time0.tv_sec)+(time1.tv_usec - time0.tv_usec)*1e-6;
	}else{
	        printf("Receiver forwarding = %f\n", (time1.tv_sec - time0.tv_sec-1)+(1e6+time1.tv_usec - time0.tv_usec)*1e-6);
	        total_time=(time1.tv_sec - time0.tv_sec-1)+(1e6+time1.tv_usec - time0.tv_usec)*1e-6;
	}
	if (time2.tv_usec >= time1.tv_usec){
	        printf("Source forwarding = %f\n", (time2.tv_sec - time1.tv_sec)+(time2.tv_usec - time1.tv_usec)*1e-6);
	        total_time+=(time2.tv_sec - time1.tv_sec)+(time2.tv_usec - time1.tv_usec)*1e-6;
	}else{
	        printf("Source forwarding = %f\n", (time2.tv_sec - time1.tv_sec-1)+(1e6+time2.tv_usec - time1.tv_usec)*1e-6);
	        total_time+=(time2.tv_sec - time1.tv_sec-1)+(1e6+time2.tv_usec - time1.tv_usec)*1e-6;
	}
	if (time3.tv_usec >= time2.tv_usec){
	        printf("Back propagation = %f\n", (time3.tv_sec - time2.tv_sec)+(time3.tv_usec - time2.tv_usec)*1e-6);
	        total_time+=(time3.tv_sec - time2.tv_sec)+(time3.tv_usec - time2.tv_usec)*1e-6;
	}else{
	        printf("Back propagation = %f\n", (time3.tv_sec - time2.tv_sec-1)+(1e6+time3.tv_usec - time2.tv_usec)*1e-6);
	        total_time+=(time3.tv_sec - time2.tv_sec-1)+(1e6+time3.tv_usec - time2.tv_usec)*1e-6;
	}

	printf("Total time = %f\n",total_time);

	clean_fields();
	clean_data();
	clean_vel();
	clean_bound();
	exit(0);
}
