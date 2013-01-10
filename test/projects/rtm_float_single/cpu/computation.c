/*
* Cpu_simulation file is part of the RTM project
* This file contains the functions required to steer the execution on the cpu
* This file contains the optimized RTM algorithm
* 1st pass - collects receiver data
* 2nd pass - propagates source forward using a wavefield with random borders
* 3rd pass - propagates receiver and source backwards and correlates both
* @author f.pratas
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "cpu_constructors.h"
#include "max_util.h"
//#define DESIGN_NAME MyApp_beg
//#include <MaxCompilerRT.h>
//#define DESIGN_NAME MyApp_ker
//#include <MaxCompilerRT.h>
//#define DESIGN_NAME MyApp_end
#include <MaxCompilerRT.h>
//#undef DESIGN_NAME

#define BURST_SIZE 384
#define FAIL(status, msg, target) \
      if (status) { printf("Err: %s\n", msg); goto target; }


extern char *device_name;

max_device_handle_t*  device[4];

max_device_handle_t*  device0;
max_maxfile_t *maxfile0;
struct max_memory_setting *ctx0;

extern float bc_b;
int Num_FPGA =1;
int Device_Num=1;
int Inter=2;//make sure this number is even
int Par=1;
int DATA_SIZE =0;
int KNL_SIZE =0;
int BYTE_SIZE =0;
int halo_SIZE=0;
int halo_size=0;
int BYTE_bursts;
int Burst_inc =1;
int PCI_width =8;
int cycle_PCI;
int step1, step2;
int xbyte, ybyte, zbyte;
long long *address_read;
long long *address_write;
int total_bursts, cycle_bursts, halo_bursts, kernel_bursts;

float *scale_glo;
float *is_glo;
float *power_glo;

struct timeval fpga_time3;
struct timeval fpga_time2;
struct timeval fpga_time1;
struct timeval fpga_time0;
int N3;

//--------------------------------------------------------------
// Max Configuration Intert
//--------------------------------------------------------------
void max_configure(){
  struct timeval tv1;
  struct timeval tv2;
  //read and write address stream shares the same stream
  int status    = 0;
  N3= (n3 - 2 * ORDER)/ Num_FPGA + 2 * ORDER;
  //N3= n3;
  int ini_bursts = N3 - 2 * ORDER;

  BYTE_SIZE = n1 * n2 * N3 * 4 * sizeof(float);//duplicate the memory for simulation
  halo_SIZE = n1 * n2 * 5 * 4 * sizeof(float);
  halo_size = n1 * n2 * ORDER;
  DATA_SIZE = n1 * n2 * N3;//N3
  KNL_SIZE  = n1 * n2 * (N3 - 2 * ORDER);//N3

  printf("ORDER:%d\n",ORDER);
  printf("N3:%d\n",N3);
  printf("DATA_SIZE:%d, BYTE_SIZE:%d\n",DATA_SIZE, BYTE_SIZE);

  printf("Opening FPGA card ... ");

  maxfile0 = max_maxfile_init_MyApp();
  max_set_maximum_runnable_timing_score(maxfile0, -1);
  device0 = max_open_device(maxfile0, device_name);
  max_set_terminate_on_error(device0);

  device[0] = device0;

  printf("done!\n");

  printf("Initialising device ... ");
  ctx0 = default_max_memory_setting(maxfile0);

  if (ctx0 == NULL ) {
    printf("Error: creating memory setting!\n");
    status = 1;
  }
  printf("done!\n");

  int d;
  for (d=0; d< Device_Num; d++)
  {
    max_set_scalar_input(device[d],"Cmdread0.Enable", 0,    FPGA_A);
    max_set_scalar_input(device[d],"Cmdread1.Enable", 0,    FPGA_A);
    max_set_scalar_input(device[d],"Cmdread2.Enable", 0,    FPGA_A);
    max_set_scalar_input(device[d],"Cmdread3.Enable", 0,    FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite0.Enable",0,    FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite1.Enable",0,    FPGA_A);
  }

  //setup address parameters
  int input_length = sizeof(float) * 4 * Par;
  total_bursts = DATA_SIZE * sizeof(float) * 4 / BURST_SIZE;       //total bursts to process
  halo_bursts  = (n1*n2*5 ) * sizeof(float) * 4 / BURST_SIZE;       //total bursts to process
  kernel_bursts= total_bursts - 2 * halo_bursts;

  cycle_bursts = BURST_SIZE / (2* input_length);                    //cycles to process one burst data
  cycle_PCI    = BURST_SIZE / (2* PCI_width);

  int unsynClock = 2 * halo_bursts * cycle_bursts;

  printf("input_length:%d, total bursts:%d,  cycle per burst:%d\n",input_length, total_bursts,cycle_bursts);
  printf("total bursts:%d, halo burst:%d, kernel burst:%d\n",total_bursts,halo_bursts,kernel_bursts);

  BYTE_bursts = BYTE_SIZE/ BURST_SIZE;       //total bursts to process
  printf("BYTE_bursts:%d\n",BYTE_bursts);


  int stream_bursts= DATA_SIZE     * sizeof(float) / BURST_SIZE;
  int stream_length= Par           * sizeof(float);
  int stream_cycles= BURST_SIZE    / stream_length;

  printf("stream bursts:%d\n",stream_bursts);
  printf("stream cycles:%d\n",stream_cycles);


  //address generator for kernels
  for (d=0; d< Device_Num; d++)
  {
    max_set_scalar_input(device[d],"Cmdread0.iniBursts",      0,                            FPGA_A);
    max_set_scalar_input(device[d],"Cmdread0.totalBursts",    stream_bursts,                FPGA_A);
    max_set_scalar_input(device[d],"Cmdread0.wordsPerBurst",  stream_cycles * Burst_inc,    FPGA_A);

    max_set_scalar_input(device[d],"Cmdread1.iniBursts",      1*stream_bursts,              FPGA_A);
    max_set_scalar_input(device[d],"Cmdread1.totalBursts",    stream_bursts,                FPGA_A);
    max_set_scalar_input(device[d],"Cmdread1.wordsPerBurst",  stream_cycles * Burst_inc,    FPGA_A);

    max_set_scalar_input(device[d],"Cmdread2.iniBursts",      2*stream_bursts,              FPGA_A);
    max_set_scalar_input(device[d],"Cmdread2.totalBursts",    stream_bursts,                FPGA_A);
    max_set_scalar_input(device[d],"Cmdread2.wordsPerBurst",  stream_cycles * Burst_inc,    FPGA_A);

    max_set_scalar_input(device[d],"Cmdread3.iniBursts",      3*stream_bursts,              FPGA_A);
    max_set_scalar_input(device[d],"Cmdread3.totalBursts",    stream_bursts,                FPGA_A);
    max_set_scalar_input(device[d],"Cmdread3.wordsPerBurst",  stream_cycles * Burst_inc,    FPGA_A);

    max_set_scalar_input(device[d],"Cmdwrite0.iniBursts",     0,                            FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite0.iterations",    Inter,                        FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite0.totalBursts",   stream_bursts,                FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite0.wordsPerBurst", stream_cycles * Burst_inc,    FPGA_A);

    max_set_scalar_input(device[d],"Cmdwrite1.iniBursts",     1*stream_bursts,              FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite1.iterations",    Inter,                        FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite1.totalBursts",   stream_bursts,                FPGA_A);
    max_set_scalar_input(device[d],"Cmdwrite1.wordsPerBurst", stream_cycles * Burst_inc,    FPGA_A);
  }

  //setup dimension parameters
  for (d=0; d< Device_Num; d++)
  {
    max_set_scalar_input(device[d],"MyApp0.n1",   n1,    FPGA_A);
    max_set_scalar_input(device[d],"MyApp0.n2",   n2,    FPGA_A);
    max_set_scalar_input(device[d],"MyApp0.n3",   N3,    FPGA_A);
    max_set_scalar_input(device[d],"MyApp0.ORDER",ORDER, FPGA_A);
    max_set_scalar_input(device[d],"MyApp0.SPONGE",SPONGE_WIDTH, FPGA_A);
  }

  //setup buffer parameters
  for (d=0; d< Device_Num; d++)
  {
    max_set_runtime_param(device[d], "MyApp0.nx",  n1/Par);
    max_set_runtime_param(device[d], "MyApp0.nxy", n1/Par*n2);
    max_upload_runtime_params(device[d], FPGA_A);
  }

  if (status)
    {
      printf("memory configuration error!\n");
      return;
    }
  printf("done!\n");

}

//--------------------------------------------------------------
// Construct 10th order space stencil
//--------------------------------------------------------------
void construct_stencil(){

	float sm=0.;
	int i;
	float d=dt;
	c_3[0]=1.66666666f*d*d/(d3*d3);
	c_2[0]=1.66666666f*d*d/(d2*d2);
	c_1[0]=1.66666666f*d*d/(d1*d1);

	c_3[1]=-.23809523f*d*d/(d3*d3);
	c_2[1]=-.23809523f*d*d/(d2*d2);
	c_1[1]=-.23809523f*d*d/(d1*d1);

	c_3[2]=.03968253f*d*d/(d3*d3);
	c_2[2]=.03968253f*d*d/(d2*d2);
	c_1[2]=.03968253f*d*d/(d1*d1);

	c_3[3]=-0.00496031f*d*d/(d3*d3);
	c_2[3]=-0.00496031f*d*d/(d2*d2);
	c_1[3]=-0.00496031f*d*d/(d1*d1);

	c_3[4]=0.00031746f*d*d/(d3*d3);
	c_2[4]=0.00031746f*d*d/(d2*d2);
	c_1[4]=0.00031746f*d*d/(d1*d1);

	for(i=0; i < 5; i++){
		sm+=c_1[i]+c_2[i]+c_3[i];
	}
	c_0=-2.0f*sm;  //Zero lag stencil

  max_configure();

#ifdef DEBUG
	fprintf(stderr,"FINISHED CONSTRUCTING STENCIL\n");
#endif

	return;
}
//--------------------------------------------------------------
// Propagation
//--------------------------------------------------------------
/*
 * do_step - propagate wavefield one step in time
 * p  - current wavefield
 * pp - previous and next wavefield
 * dvv - density (1) * velocity *velocity
 */
void do_step_damping(float *__restrict p, float *__restrict pp, float *__restrict dvv, float *__restrict source_container){
	int i3;
	int n12=n1*n2;

#pragma omp parallel for
	for(i3=ORDER; i3 < n3-ORDER; i3++){   //Loop over slowest axis
		int i1;
		int i2;
		float f1;//Distance from the boundary
		float f2;
		float f3;

		if(i3 < SPONGE_WIDTH) f3=SPONGE_WIDTH-i3;  //Inside neg boundary
		else if(i3 >= n3-SPONGE_WIDTH)  f3=i3-(n3-SPONGE_WIDTH)+1; //Inside pos boundary
		else f3=0; // Not in boundary region
		f3=f3*f3;  //Distance in boundary

		for(i2=ORDER; i2 < n2-ORDER; i2++){ //Loop over middle axis
			if(i2 < SPONGE_WIDTH) f2=SPONGE_WIDTH-i2;  // Inside neg boundary
			else if(i2 >= n2-SPONGE_WIDTH)  f2=i2-(n2-SPONGE_WIDTH)+1;  //Inside pos boundary
			else f2=0; //Not in boundary
			f2=f2*f2;  //Distance in boundary

			for(i1=ORDER; i1< n1-ORDER; i1++){  //Loop over fast axis
				int is;
				float scale;

				if(i1 < BOUND_T) f1=(BOUND_T-i1); //Inside neg boundary
				else if(i1 >= n1-SPONGE_WIDTH)  f1=(i1-(n1-SPONGE_WIDTH)+1);//Inside pos bound
				else f1=0;  // Not in boundary
				f1=f1*f1;   //Distance in boundary

				is=(int)(sqrtf(f1+f2+f3));  //Distance from edge
				if(is>SPONGE_WIDTH) is=SPONGE_WIDTH;  //Don't go outside computed boundary region

        is_glo[i1+i2*n1+i3*n12] = is;
        power_glo[i1+i2*n1+i3*n12] = is * (-bc_b);

				if(is >0) scale=bound_e[SPONGE_WIDTH-is]; //get scale parameter
				else scale=1;

				pp[i1+i2*n1+i3*n12]*=scale; // sponge previous wavefield

				//Wavefield update
				pp[i1+i2*n1+i3*n12]=(2.0*p[i1+i2*n1+i3*n12]-pp[i1+i2*n1+i3*n12]+dvv[i1+i2*n1+i3*n12]*(
						p[i1+i2*n1+i3*n12]*c_0
						+c_1[0]*(p[(i1+1)+(i2  )*n1+(i3  )*n12]+p[(i1-1)+(i2  )*n1+(i3  )*n12])
						+c_1[1]*(p[(i1+2)+(i2  )*n1+(i3  )*n12]+p[(i1-2)+(i2  )*n1+(i3  )*n12])
						+c_1[2]*(p[(i1+3)+(i2  )*n1+(i3  )*n12]+p[(i1-3)+(i2  )*n1+(i3  )*n12])
						+c_1[3]*(p[(i1+4)+(i2  )*n1+(i3  )*n12]+p[(i1-4)+(i2  )*n1+(i3  )*n12])
						+c_1[4]*(p[(i1+5)+(i2  )*n1+(i3  )*n12]+p[(i1-5)+(i2  )*n1+(i3  )*n12])
						+c_2[0]*(p[(i1  )+(i2+1)*n1+(i3  )*n12]+p[(i1  )+(i2-1)*n1+(i3  )*n12])
						+c_2[1]*(p[(i1  )+(i2+2)*n1+(i3  )*n12]+p[(i1  )+(i2-2)*n1+(i3  )*n12])
						+c_2[2]*(p[(i1  )+(i2+3)*n1+(i3  )*n12]+p[(i1  )+(i2-3)*n1+(i3  )*n12])
						+c_2[3]*(p[(i1  )+(i2+4)*n1+(i3  )*n12]+p[(i1  )+(i2-4)*n1+(i3  )*n12])
						+c_2[4]*(p[(i1  )+(i2+5)*n1+(i3  )*n12]+p[(i1  )+(i2-5)*n1+(i3  )*n12])
						+c_3[0]*(p[(i1  )+(i2  )*n1+(i3+1)*n12]+p[(i1  )+(i2  )*n1+(i3-1)*n12])
						+c_3[1]*(p[(i1  )+(i2  )*n1+(i3+2)*n12]+p[(i1  )+(i2  )*n1+(i3-2)*n12])
						+c_3[2]*(p[(i1  )+(i2  )*n1+(i3+3)*n12]+p[(i1  )+(i2  )*n1+(i3-3)*n12])
						+c_3[3]*(p[(i1  )+(i2  )*n1+(i3+4)*n12]+p[(i1  )+(i2  )*n1+(i3-4)*n12])
						+c_3[4]*(p[(i1  )+(i2  )*n1+(i3+5)*n12]+p[(i1  )+(i2  )*n1+(i3-5)*n12])
						))+source_container[i1+i2*n1+i3*n12];
				pp[i1+i2*n1+i3*n12]*=scale; // sponge result
        scale_glo[i1+i2*n1+i3*n12] = scale;
			}
		}
	}
	return;
}
void do_step(float *__restrict p, float *__restrict pp, float *__restrict dvv, float *__restrict source_container){
	int i1,i2,i3;  //Indexes
	int n12=n1*n2;
  float *conv     =(float*)malloc(sizeof(float)*n123);
  float *cal      =(float*)malloc(sizeof(float)*n123);


  #pragma omp parallel for
    for(i3=ORDER; i3 < n3-ORDER; i3++){   //Loop over slowest axis
      int i1;
      int i2;
  		for(i2=ORDER; i2 < n2-ORDER; i2++){ //Loop over middle axis
  			for(i1=ORDER; i1< n1-ORDER; i1++){  //Loop over fast axis
    		//Wavefield update
    		  conv[i1+i2*n1+i3*n12]=dvv[i1+i2*n1+i3*n12]*(
    				p[i1+i2*n1+i3*n12]*c_0
    				+c_1[0]*(p[(i1+1)+(i2  )*n1+(i3  )*n12]+p[(i1-1)+(i2  )*n1+(i3  )*n12])
    				+c_1[1]*(p[(i1+2)+(i2  )*n1+(i3  )*n12]+p[(i1-2)+(i2  )*n1+(i3  )*n12])
    				+c_1[2]*(p[(i1+3)+(i2  )*n1+(i3  )*n12]+p[(i1-3)+(i2  )*n1+(i3  )*n12])
    				+c_1[3]*(p[(i1+4)+(i2  )*n1+(i3  )*n12]+p[(i1-4)+(i2  )*n1+(i3  )*n12])
    				+c_1[4]*(p[(i1+5)+(i2  )*n1+(i3  )*n12]+p[(i1-5)+(i2  )*n1+(i3  )*n12])
    				+c_2[0]*(p[(i1  )+(i2+1)*n1+(i3  )*n12]+p[(i1  )+(i2-1)*n1+(i3  )*n12])
    				+c_2[1]*(p[(i1  )+(i2+2)*n1+(i3  )*n12]+p[(i1  )+(i2-2)*n1+(i3  )*n12])
    				+c_2[2]*(p[(i1  )+(i2+3)*n1+(i3  )*n12]+p[(i1  )+(i2-3)*n1+(i3  )*n12])
    				+c_2[3]*(p[(i1  )+(i2+4)*n1+(i3  )*n12]+p[(i1  )+(i2-4)*n1+(i3  )*n12])
    				+c_2[4]*(p[(i1  )+(i2+5)*n1+(i3  )*n12]+p[(i1  )+(i2-5)*n1+(i3  )*n12])
    				+c_3[0]*(p[(i1  )+(i2  )*n1+(i3+1)*n12]+p[(i1  )+(i2  )*n1+(i3-1)*n12])
    				+c_3[1]*(p[(i1  )+(i2  )*n1+(i3+2)*n12]+p[(i1  )+(i2  )*n1+(i3-2)*n12])
    				+c_3[2]*(p[(i1  )+(i2  )*n1+(i3+3)*n12]+p[(i1  )+(i2  )*n1+(i3-3)*n12])
    				+c_3[3]*(p[(i1  )+(i2  )*n1+(i3+4)*n12]+p[(i1  )+(i2  )*n1+(i3-4)*n12])
    				+c_3[4]*(p[(i1  )+(i2  )*n1+(i3+5)*n12]+p[(i1  )+(i2  )*n1+(i3-5)*n12]));

          cal[i1+i2*n1+i3*n12]=2.0*p[i1+i2*n1+i3*n12]-pp[i1+i2*n1+i3*n12];

    			pp[i1+i2*n1+i3*n12]=cal[i1+i2*n1+i3*n12] + conv[i1+i2*n1+i3*n12] + source_container[i1+i2*n1+i3*n12];

//        printf("i3-i2-i1:%d-%d-%d\n",i3,i2,i1);
//        printf("pp:%.18f\n",pp[i1+i2*n1+i3*n12]);
//        printf("p:%.18f\n",p[i1+i2*n1+i3*n12]);
//        printf("dvv:%.18f\n",dvv[i1+i2*n1+i3*n12]);
//        printf("source:%.18f\n",source_container[i1+i2*n1+i3*n12]);
  			}
  		}
  	}

  free(conv);
  free(cal);

  return;
}
/*
 * add_source - add source to wavefield
 * it - current time sample
 * p - current wavefield
 */
void add_source(int it, float *__restrict source_container){
	int i1,i2,i3;
	float x1,x2,x3;
	float x,xx;
	float val,tdelay;

  tdelay = 1./fpeak;
	float t=(float)it*dt;
	if(t > 2.*tdelay) {
		clean_source();
	}else{
		x = 3.1415926536*fpeak*(t-tdelay);
		xx = x*x;
		val=expf(-xx)*(1.0-2.0*xx); //Wavelet time

		//Add source term over a region
		for(i3=isy-1; i3 <=isy+1; i3++){
			x3=isy-i3;
			for(i2=isx-1; i2 <=isx+1; i2++){
				x2=isx-i2;
				for(i1=isz-1; i1 <= isz+1; i1++){
					x1=isz-i1;
					source_container[i1+i2*n1+i3*n1*n2]=-val*expf(-x1*x1-x2*x2-x3*x3);
				}
			}
		}
	}
	return;
}
/*
 * extract_data - extract data from forward propagation in this case everywhere on the surface
 * it - current time sample
 * p  - current wavefield
 */
void extract_data(int ifr,float *__restrict p,float *__restrict frames_container){
	int i2,i3;

#pragma omp parallel for default(shared) private(i3,i2)
	for(i3=0; i3 < n3; i3++){
		for(i2=0; i2 < n2; i2++){
			frames_container[i2+i3*n2+ifr*n2*n3]=p[isz+i2*n1+i3*n1*n2];
		}
	}
}
/*
 * prop_source - propagation source wavefield
 * model - whether or not I am modeling data or propagating source wavefield
 */
void prop_source(int model){
	int it;
	long long i;
	float *p1,*p2,*p_aux;

  int i1,i2,i3;  //Indexes
	int n12=n1*n2;
  int ii, jj;

  float *got  = (float*)malloc(BYTE_SIZE);
  float *comb0= (float*)malloc(BYTE_SIZE);
  float *comb1= (float*)malloc(BYTE_SIZE);
  float *comb2= (float*)malloc(BYTE_SIZE);
  float *comb3= (float*)malloc(BYTE_SIZE);

  is_glo=(float*)malloc(sizeof(float)*DATA_SIZE * Num_FPGA);
  scale_glo=(float*)malloc(sizeof(float)*DATA_SIZE * Num_FPGA);
  power_glo=(float*)malloc(sizeof(float)*DATA_SIZE * Num_FPGA);


#ifdef DEBUG
	FILE *fd;
	char output[64];
	strcpy(output,folder_path);


  if(model){
		strcat(output,"/rec_prop.out");
		//saving receiver
		create_header_file(output);

		fd = fopen(output,"w+");
		if(fd == NULL){
			fprintf(stderr,"Error: couldn't open rec_prop.out\n");
			exit(-1);
		}
	}else{
		strcat(output,"/src_prop.out");
		//saving source
		create_header_file(output);

		fd = fopen(output,"w+");
		if(fd == NULL){
			fprintf(stderr,"Error: couldn't open src_prop.out\n");
			exit(-1);
		}
	}
#endif

	if(model){p1=pon_g;p2=pc_g;}
	else{p1=pon_s;p2=pc_s;}

	//Loop over time
	for(it=0; it <1; it++){
		add_source(it,source);
    printf("it:%d\n",it);
		if(model){
			//use the damping field to get the receiver data
			do_step_damping(p2,p1,vel_g,source);
			if(it%jt==0) extract_data((int)(it/jt),p1,data);
		  p_aux=p1; p1=p2; p2=p_aux; //Flip pointers
      it++;

      do_step_damping(p2,p1,vel_g,source);
			if(it%jt==0) extract_data((int)(it/jt),p1,data);
		  p_aux=p1; p1=p2; p2=p_aux; //Flip pointers
      it++;

      do_step_damping(p2,p1,vel_g,source);
			if(it%jt==0) extract_data((int)(it/jt),p1,data);
		  p_aux=p1; p1=p2; p2=p_aux; //Flip pointers
      it++;

			do_step_damping(p2,p1,vel_g,source);
			if(it%jt==0) extract_data((int)(it/jt),p1,data);

    }else{

    //kernel part

    i=0;
    for(i3=0; i3 < N3; i3++)     //Loop over slowest axis
  		for(i2=0; i2 < n2; i2++)   //Loop over middle axis
  			for(i1=0; i1< n1; i1++){  //Loop over fast axis
          comb0[i]  =         p2[i1+i2*n1+i3*n12];
          i++;
        }
    for(i3=0; i3 < N3; i3++)     //Loop over slowest axis
  		for(i2=0; i2 < n2; i2++)   //Loop over middle axis
  			for(i1=0; i1< n1; i1++){  //Loop over fast axis
          comb0[i]  =         p1[i1+i2*n1+i3*n12];
          i++;
        }
    for(i3=0; i3 < N3; i3++)     //Loop over slowest axis
  		for(i2=0; i2 < n2; i2++)   //Loop over middle axis
  			for(i1=0; i1< n1; i1++){  //Loop over fast axis
          comb0[i]  =         vel_s[i1+i2*n1+i3*n12];
          i++;
        }
    for(i3=0; i3 < N3; i3++)     //Loop over slowest axis
  		for(i2=0; i2 < n2; i2++)   //Loop over middle axis
  			for(i1=0; i1< n1; i1++){  //Loop over fast axis
          comb0[i]  =         source[i1+i2*n1+i3*n12];
          i++;
        }
      //FPGA-0
      max_kernel_set_cycles(device0,"Cmdhostwrite",         BYTE_bursts * cycle_PCI,   FPGA_A);
      max_set_scalar_input( device0,"Cmdhostwrite.iterations",   1,                    FPGA_A);
      max_set_scalar_input( device0,"Cmdhostwrite.totalBursts",  BYTE_bursts,          FPGA_A);
      max_set_scalar_input( device0,"Cmdhostwrite.wordsPerBurst",cycle_PCI * Burst_inc,FPGA_A);

      max_set_scalar_input( device0,"Cmdhostwrite.Enable",       1,                    FPGA_A);
      max_memory_stream_interrupt_on(ctx0, "mgr2dram", NULL);
      max_run(device0, max_input("host2mgr", comb0, BYTE_SIZE), max_end());
      max_wait_for_interrupt(device0, FPGA_A);
      max_set_scalar_input(device0,"Cmdhostwrite.Enable",         0,                   FPGA_A);
      printf("FPGA-0 cpu to memory\n");

      //FPGA execute
        gettimeofday(&fpga_time0,NULL);
        int d;
        for (d=0; d< Device_Num; d++)
        {
        max_kernel_set_cycles(device[d],"MyApp0",   Inter * DATA_SIZE/ Par, FPGA_A);
        max_kernel_set_cycles(device[d],"Cmdread0", Inter * DATA_SIZE/ Par, FPGA_A);
        max_kernel_set_cycles(device[d],"Cmdread1", Inter * DATA_SIZE/ Par, FPGA_A);
        max_kernel_set_cycles(device[d],"Cmdread2", Inter * DATA_SIZE/ Par, FPGA_A);
        max_kernel_set_cycles(device[d],"Cmdread3", Inter * DATA_SIZE/ Par, FPGA_A);
        max_kernel_set_cycles(device[d],"Cmdwrite0",Inter * DATA_SIZE/ Par, FPGA_A);
        max_kernel_set_cycles(device[d],"Cmdwrite1",Inter * DATA_SIZE/ Par, FPGA_A);

        max_set_scalar_input(device[d], "Cmdread0.Enable", 1, FPGA_A);
        max_set_scalar_input(device[d], "Cmdread1.Enable", 1, FPGA_A);
        max_set_scalar_input(device[d], "Cmdread2.Enable", 1, FPGA_A);
        max_set_scalar_input(device[d], "Cmdread3.Enable", 1, FPGA_A);
        max_set_scalar_input(device[d], "Cmdwrite0.Enable",1, FPGA_A);
        max_set_scalar_input(device[d], "Cmdwrite1.Enable",1, FPGA_A);
        }

        max_memory_stream_interrupt_on(ctx0, "knl2dram0", NULL);
        max_memory_stream_interrupt_on(ctx0, "knl2dram1", NULL);
        max_reset_device(device0);
        max_wait_for_interrupt(device0, FPGA_A);
        max_wait_for_interrupt(device0, FPGA_A);

        for (d=0; d< Device_Num; d++)
        {
        max_set_scalar_input(device[d], "Cmdread0.Enable", 0, FPGA_A);
        max_set_scalar_input(device[d], "Cmdread1.Enable", 0, FPGA_A);
        max_set_scalar_input(device[d], "Cmdread2.Enable", 0, FPGA_A);
        max_set_scalar_input(device[d], "Cmdread3.Enable", 0, FPGA_A);
        max_set_scalar_input(device[d], "Cmdwrite0.Enable",0, FPGA_A);
        max_set_scalar_input(device[d], "Cmdwrite1.Enable",0, FPGA_A);
        }
        printf("kernel finished\n");

        gettimeofday(&fpga_time1,NULL);
        printf("FPGA Computing = %f\n", (fpga_time1.tv_sec  - fpga_time0.tv_sec)
                                       +(fpga_time1.tv_usec - fpga_time0.tv_usec)*1e-6);

      max_kernel_set_cycles(device0,"Cmdhostread",          BYTE_bursts * cycle_PCI,         FPGA_A);
      max_set_scalar_input( device0,"Cmdhostread.totalBursts",     BYTE_bursts,              FPGA_A);
      max_set_scalar_input( device0,"Cmdhostread.wordsPerBurst", cycle_PCI * Burst_inc,      FPGA_A);
      max_set_scalar_input( device0,"Cmdhostread.Enable",         1,               FPGA_A);
      max_run(device0, max_input("mgr2host", comb0, BYTE_SIZE), max_end());
      max_set_scalar_input( device0,"Cmdhostread.Enable",         0,               FPGA_A);

      gettimeofday(&fpga_time3,NULL);
      printf("FPGA overall = %f\n",(fpga_time3.tv_sec  - fpga_time2.tv_sec)
                                  +(fpga_time3.tv_usec - fpga_time2.tv_usec)*1e-6);



      for (i=0;i<Inter-1;i++)
      {
        do_step(p2,p1,vel_s,source);
        p_aux=p1; p1=p2; p2=p_aux; //Flip pointers
        it++;
      }

      do_step(p2,p1,vel_s,source);

      i=0;
      for(i3=0; i3 < N3; i3++)     //Loop over slowest axis
  		  for(i2=0; i2 < n2; i2++)   //Loop over middle axis
  			  for(i1=0; i1< n1; i1++)  //Loop over fast axis
          {
             if (comb0[i]!=p1[i1+i2*n1+i3*n12])
             {
               printf("%d  %d  %d\n",i3, i2, i1);
               printf("expected:%.28f, got:%.28f\n",p1[i1+i2*n1+i3*n12],comb0[i]);
             }
             i++;
          }
  }

#ifdef DEBUG
			if(it%jt==0) fwrite(&(p1[isy*n2*n1]), sizeof(float), n1*n2, fd);
		//Print progress
		switch(it%100){
			case 0: fprintf(stderr," Propagating  Forward %5d of %5d |\r",it,nt);break;
			case 25: fprintf(stderr," Propagating  Forward %5d of %5d /\r",it,nt);break;
			case 50: fprintf(stderr," Propagating  Forward %5d of %5d -\r",it,nt);break;
			case 75: fprintf(stderr," Propagating  Forward %5d of %5d \\\r",it,nt);
		}
#endif
		p_aux=p1; p1=p2; p2=p_aux; //Flip pointers
	}
	if((nt%2)==1){  //Make sure pointers are consistent for backward propagation
		if(model){p_aux=pon_g; pon_g=pc_g; pc_g=p_aux;} //Flip pointers
		else{p_aux=pon_s; pon_s=pc_s; pc_s=p_aux;} //Flip pointers
	}
#ifdef DEBUG
	fprintf(stderr," Propagating  Forward %5d of %5d X\n",it,nt);
	fclose(fd);
#endif
	return;
}
//--------------------------------------------------------------
// Take time derivative of data and mute out direct arrival
//--------------------------------------------------------------
void differentiate_mute_data(char *file){
	int i3;
	int wavelet_samp;
	wavelet_samp = (2/(fpeak*dt))/jt;

	float vel=sqrt(vel_g[isz+isx*n1+isy*n1*n2]);

#ifdef DEBUG
	FILE *fd;
	char output[64];
	strcpy(output,folder_path);
	strcat(output,"/rec_data_or.out");
	//receiver data with source
	create_header_file(output);
	fd = fopen(output,"w+");
	if(fd == NULL){
		fprintf(stderr,"Error: couldn't open rec_data_or.out\n");
		exit(-1);
	}
	fwrite(data, sizeof(float), nf*n3*n2, fd);
	fclose(fd);
	strcpy(output,folder_path);
	strcat(output,"/rec_data.out");
	//receiver data without source
	create_header_file(output);
	fd = fopen(output,"w+");
	if(fd == NULL){
		fprintf(stderr,"Error: couldn't open rec_data.out\n");
		exit(-1);
	}
#endif

#pragma omp parallel for
	for(i3=0; i3 <n3; i3++){
		int i2;
		int ifr;
		for(i2=0; i2 < n2; i2++){
			//Distance away from source
			float dist=sqrt((isx-i2)*(isx-i2)*d2*d2+d3*d3*(isy-i3)*(isy-i3));
			int mute_s=((dist/vel)/(dt*jt)); //Mute frames
			mute_s+=wavelet_samp; //remove whole source (the source is injected during number of steps = 2/(fpeak*dt))
			for(ifr=0; (ifr < mute_s) && (ifr < nf); ifr++) data[i2+i3*n2+ifr*n2*n3]=0; //Mute direct arrive
		}
	}

	save_receiver_to_file(file);

#ifdef DEBUG
	fwrite(data, sizeof(float), nf*n3*n2, fd);
	fclose(fd);
#endif

	return;
}
//--------------------------------------------------------------
// Migrate a shot
//--------------------------------------------------------------
/*
 * image_it  - Apply imaging condition
 * p1 - source wavefield
 * p2 - receiver wavefield
 */
void image_it(float *__restrict source_container,float *__restrict receiver,float **__restrict image_container){
	int k;

	//2D gather
#pragma omp parallel for
	for(k=0; k < n3; k++){
		int j;
		int i;
		int sub;
		for(sub=0; sub<=num_subsurface_offsets; sub++){
			for(j=sub; j < n2-sub; j++){
				for(i=0; i < n1; i++){
					image_container[sub][i+j*n1+k*n1*n2]+=source_container[i+(j-sub)*n1+k*n1*n2]*receiver[i+(j+sub)*n1+k*n1*n2];
				}
			}
		}
	}
	return;
}
/*
 * add_data - add data to propagation grid
 * it - current time step
 * p - current wavefield
 */
void add_data(int it,float *__restrict source_container, float *__restrict frames_container){
	int i2,i3;
	int ib=it/jt; //Linear interpolation
	int ie=ib+1;
	float f=(float)(it-ib*jt)/(float)(jt);
	if(ie>=nf) { ie=nf-1; f=1.;}
	int n12=n1*n2;

#pragma omp parallel for
	for(i3=0; i3 < n3; i3++){
		int i2;
		for(i2=0; i2 < n2; i2++){
			source_container[isz+i2*n1+i3*n12]=(1.-f)*frames_container[i2+i3*n2+ib*n2*n3]+f*frames_container[i2+i3*n2+ie*n2*n3];
		}
	}
	return;
}
/*
 * migrate_shot - Migrate a single shot
 */
void migrate_shot(){
	int it;
	float *p1s,*p1g,*p_aux,*p2s,*p2g;
	p1s=pon_s;
	p2s=pc_s;
	p1g=pon_g;
	p2g=pc_g;

#ifdef DEBUG
	FILE *fd1,*fd2;
	char output[64];
	strcpy(output,folder_path);
	strcat(output,"/rec_bprop.out");
	create_header_file(output);
    fd1 = fopen(output,"w+");
    if(fd1 == NULL){
    	fprintf(stderr,"Error: couldn't open rec_bprop.out\n");
    	exit(-1);
    }
	strcpy(output,folder_path);
	strcat(output,"/src_bprop.out");
	create_header_file(output);
    fd2 = fopen(output,"w+");
    if(fd2 == NULL){
    	fprintf(stderr,"Error: couldn't open src_bprop.out\n");
    	exit(-1);
    }
#endif

	//Should loop from max time to 0
	for(it=0; it>=0; it--){

#ifdef DEBUG
		//Print progress
		switch(it%100){
			case 0: fprintf(stderr," Propagating Backward %5d of %5d |\r",it,nt);break;
			case 25: fprintf(stderr," Propagating Backward %5d of %5d /\r",it,nt);break;
			case 50: fprintf(stderr," Propagating Backward %5d of %5d -\r",it,nt);break;
			case 75: fprintf(stderr," Propagating Backward %5d of %5d \\\r",it,nt);
		}
#endif

		clean_source();
		add_source(it,source);
		add_data(it,source,data);  	      		    //Inject recorded data
//  float *got     =(float*)malloc(sizeof(float)*DATA_SIZE);
//  float *got_bak =(float*)malloc(sizeof(float)*DATA_SIZE);
//  float *got_img =(float*)malloc(sizeof(float)*DATA_SIZE*(num_subsurface_offsets+1));
//  float *comb=(float*)malloc(BYTE_SIZE);
//  int i = 0;
//  int i1, i2, i3, i0;
//  int n12 = n1 * n2;
//  for(i3=0; i3 < n3; i3++)     //Loop over slowest axis
//		for(i2=0; i2 < n2; i2++)   //Loop over middle axis
//			for(i1=0; i1< n1; i1+=Par){  //Loop over fast axis
//        for(i0=0; i0<Par; i0++)
//          {
//           comb[i*Par*8 +Par*0 +i0]  =        p2s[i1+i0  +i2*n1+i3*n12];
//           comb[i*Par*8 +Par*1 +i0]  =        p1s[i1+i0  +i2*n1+i3*n12];
//           comb[i*Par*8 +Par*2 +i0]  =      vel_s[i1+i0  +i2*n1+i3*n12];
//           comb[i*Par*8 +Par*3 +i0]  =     source[i1+i0  +i2*n1+i3*n12];
//           comb[i*Par*8 +Par*4 +i0]  =        p2g[i1+i0  +i2*n1+i3*n12];
//           comb[i*Par*8 +Par*5 +i0]  =        p1g[i1+i0  +i2*n1+i3*n12];
//           comb[i*Par*8 +Par*6 +i0]  =      vel_g[i1+i0  +i2*n1+i3*n12];
//           comb[i*Par*8 +Par*7 +i0]  =     image[0][i1+i0+i2*n1+i3*n12];
//          }
//           i++;
//      }
//  MAX_PCI2Mem((void *)comb,  BYTE_SIZE, "host2mgr", "mgr2dram");
//  printf("cpu to memory\n");
//  //FPGA execute
//    gettimeofday(&fpga_time0,NULL);
//    max_set_scalar_input(device,"Cmdread.Enable",         1,               FPGA_A);
//    max_set_scalar_input(device,"Cmdwrite.Enable",        1,               FPGA_A);
//    max_memory_stream_interrupt_on(ctx, "knl2dram", NULL);
//    max_reset_device(device);
//    max_wait_for_interrupt(device, FPGA_A);
//    max_set_scalar_input(device,"Cmdread.Enable",         0,               FPGA_A);
//    max_set_scalar_input(device,"Cmdwrite.Enable",        0,               FPGA_A);
//    printf("kernel finished\n");
//    gettimeofday(&fpga_time1,NULL);
//    printf("FPGA Computing = %f\n", (fpga_time1.tv_sec  - fpga_time0.tv_sec)
//                                   +(fpga_time1.tv_usec - fpga_time0.tv_usec)*1e-6);
//  MAX_Mem2PCI((void *)comb, BYTE_SIZE, "mgr2host", "dram2mgr");
//  printf("memory to cpu\n");

//  //time-step 1
//do_step(p1s,p2s,vel_s,source); 		//Backward prop source
////add_data(it,source,data);  	      		    //Inject recorded data
//do_step_damping(p1g,p2g,vel_g,source); //Backward prop receiver
//if(it%jt==0) image_it(p2s,p2g,image); //Apply imaging condition
//fdef DEBUG
//if(it%jt==0) fwrite(&(p2g[(n3/2)*n2*n1]), sizeof(float), n1*n2, fd1);
//if(it%jt==0) fwrite(&(p2s[(n3/2)*n2*n1]), sizeof(float), n1*n2, fd2);
//ndif
//p_aux=p1s; p1s=p2s; p2s=p_aux;  		//Flip source pointers
//p_aux=p1g; p1g=p2g; p2g=p_aux;  		//Flip receiver pointers
//
//  //time-step 2
//  do_step(p1s,p2s,vel_s,source); 		//Backward prop source
//  //add_data(it,source,data);  	      		    //Inject recorded data
//  do_step_damping(p1g,p2g,vel_g,source); //Backward prop receiver
//  if(it%jt==0) image_it(p2s,p2g,image); //Apply imaging condition
//  #ifdef DEBUG
//  if(it%jt==0) fwrite(&(p2g[(n3/2)*n2*n1]), sizeof(float), n1*n2, fd1);
//  if(it%jt==0) fwrite(&(p2s[(n3/2)*n2*n1]), sizeof(float), n1*n2, fd2);
//  #endif
//  p_aux=p1s; p1s=p2s; p2s=p_aux;  		//Flip source pointers
//  p_aux=p1g; p1g=p2g; p2g=p_aux;  		//Flip receiver pointers


//    int j;
//    //to pick out block_pp
//    for(i=0;i<n123/Par;i++)
//     for(j=0; j<Par; j++)
//    {
//      got[i*Par+j]        =comb[Par*8*i+j];
//      got_bak[i*Par+j]    =comb[Par*8*i+Par*4+j];
//      got_img[i*Par+j]    =comb[Par*8*i+Par*7+j];
//    }

//    for(i3=0; i3 < n3; i3++)     //Loop over slowest axis
//		  for(i2=0; i2 < n2; i2++)   //Loop over middle axis
//			  for(i1=0; i1< n1; i1++)  //Loop over fast axis
//             if (got[i1+i2*n1+i3*n12]!=p1s[i1+i2*n1+i3*n12])
//             {
//               printf("error:%d  %d  %d\n",i3, i2, i1);
//               printf("expected:%.22f, got:%.22f\n",p1s[i1+i2*n1+i3*n12],got[i1+i2*n1+i3*n12]);
//             }

//    for(i3=0; i3 < n3; i3++)     //Loop over slowest axis
//		  for(i2=0; i2 < n2; i2++)   //Loop over middle axis
//			  for(i1=0; i1< n1; i1++)  //Loop over fast axis
//             if (got_bak[i1+i2*n1+i3*n12]!=p1g[i1+i2*n1+i3*n12])
//             {
//               printf("error bak:%d  %d  %d\n",i3, i2, i1);
//               printf("expected:%.22f, got:%.22f, power:%.22f, is:%.22f, scale:%.22f\n"
//                      ,p1g[i1+i2*n1+i3*n12],got_bak[i1+i2*n1+i3*n12], power_glo[i1+i2*n1+i3*n12],
//                       is_glo[i1+i2*n1+i3*n12], scale_glo[i1+i2*n1+i3*n12]);
//             }
//
//    for(i3=0; i3 < n3; i3++)     //Loop over slowest axis
//		  for(i2=0; i2 < n2; i2++)   //Loop over middle axis
//			  for(i1=0; i1< n1; i1++)  //Loop over fast axis
//             if (got_img[i1+i2*n1+i3*n12]!=image[0][i1+i2*n1+i3*n12])
//             {
//               printf("error image:%d  %d  %d\n",i3, i2, i1);
//               printf("expected:%.22f, got:%.22f\n",image[0][i1+i2*n1+i3*n12],got_img[i1+i2*n1+i3*n12]);
//             }
	}

#ifdef DEBUG
	fprintf(stderr," Propagating Backward %5d of %5d X\n",0,nt);
	fclose(fd1);
	fclose(fd2);
#endif
	return;
}
