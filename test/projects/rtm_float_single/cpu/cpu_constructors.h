#define ORDER 5 //Order of stencil
// Globals
extern char *folder_path;
extern int BOUND_T;      //Boundary on top - 60 is probably a good number for random
extern int SPONGE_WIDTH; //Boundary on all other edges - 50 probably should be dropped to 40
extern int TOTAL_SPONGE_WIDTH;
extern int n1,n2,n3;    //Size of wavefield and data
extern int nf;          //Number of frames
extern int nt;          //Number of time steps
extern long long n123;   //Number of elements
extern float d1,d2,d3;  //Sampling of wavefield
extern float dscale;	 //Scale of the earth model in meters/point
extern float dt; 		 //Time scale in seconds/time step
extern int jt;  		 //Number of time steps per frame of output
extern float fpeak; 	 //Maximum frequency
extern int isx,isy, isz;  	 //Source location
extern float c_3[5],c_2[5],c_1[5],c_0;  //Stencil coefficients
extern float vmax, vmin; // max and min velocity
extern int num_subsurface_offsets, cross3D;
extern int rerun_receiver, rerun_source;
extern int irand;

//extern float bc_a,bc_b;  //Damping parameters
//extern float d1;		 //Sampling of wavefield
// Parameters
char * create_params(char *);
// Wavefields
extern float *pon_s,*pc_s,*pc_g,*pon_g; //Wavefield fields
extern float *source;
extern float **image; //Image
void create_fields();
void clean_receiver_field();
void clean_source();
void clean_fields();
// Data
extern float *data;  //Data
void create_data();
void clean_data();
int get_receiver_from_file(char *);
void save_receiver_to_file(char *);
// Velocity
extern float *vel_s, *vel_g;  //Velocity with and without random bounds
void read_earth_model(char *);
void create_vel();
void clean_vel();
// Random boundary
void random_bound_dvv();
// Damping region
extern float *bound_e;  //Boundary arrays
void construct_bound();
void clean_bound();
// Square earth model dvv
void square_earth_model();
// Dump Image to File
void create_header_file(char *);
void dump_image_to_file(char *);
// Take derivative of image to clear up artifacts
void deriv_image();
