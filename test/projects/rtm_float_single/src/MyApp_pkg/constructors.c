/*
* Constructors file is part of the RTM project
* This file contains functions and variables that are common to all the devices
* Namely memory allocations and initializations
* @author f.pratas
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

//--------------------------------------------------------------
//Global variables
//--------------------------------------------------------------
char *folder_path;
int BOUND_T;     //Boundary on top - 60 is probably a good number for random
int SPONGE_WIDTH;//Boundary on all other edges - 50 probably should be dropped to 40
int TOTAL_SPONGE_WIDTH;
int n1;//Size of wavefield and data z
int n2;//Size of wavefield and data x
int n3;//Size of wavefield and data y
int nf;          //Number of frames
int nt;          //Number of time steps
long long n123;  //Number of elements
float bc_b; //Damping parameters
float d1;//Sampling of wavefield
float d2;//Sampling of wavefield
float d3;//Sampling of wavefield
float dscale;	 //Scale of the earth model in meters/point
float dt; 		 //Time scale in seconds/time step
int jt;  		 //Number of time steps per frame of output
float fpeak; 	 //Maximum frequency
int isx;//Source location
int isy;
int isz;
float c_3[5];//Stencil coefficients
float c_2[5];
float c_1[5];
float c_0;
float vmax;// max and min velocity
float vmin;
int num_subsurface_offsets;
int rerun_receiver;
int   irand; // random seed for initialisation of the random boundary
//--------------------------------------------------------------
// Initial parameters
//
// This function reads the input parameter file and initializes
// the corresponding global variables (dimensions, frequency, source position etc.),
// it also checks for inconsistencies in these variables
//--------------------------------------------------------------
char * create_params(char *params_file){
	FILE *fd;
	char * earth_model_file=NULL;
	char buff1[256];
	int count;

	fd = fopen(params_file,"r");
	if(fd == NULL){
		fprintf(stderr,"Error: invalid parameter input file '%s' !\n",params_file);
		exit(-1);
	}

	isz=-1;
	isx=-1;
	isy=-1;

	count = 0;
	while(fscanf(fd,"%s\n",buff1)!=EOF){
		//comment
		if(buff1[0]=='#')continue;

		else if(!strcmp(buff1,"EMF")){
			if((fscanf(fd,"%s\n",buff1)==EOF) || buff1[0]=='#'){
				fprintf(stderr,"Error: wrong EMF parameter or empty\n");
				exit(-1);
			}
			earth_model_file=(char*)malloc(sizeof(char)*(strlen(buff1)+1+strlen("../tests/data/")));
			if(earth_model_file==NULL){
				fprintf(stderr,"Error: allocating string with earth model name file path\n");
				exit(-1);
			}
			strcpy(earth_model_file,"../tests/data/");
			strcat(earth_model_file,buff1);
		}else if(!strcmp(buff1,"OUT_FOLDER")){
			if((fscanf(fd,"%s\n",buff1)==EOF) || buff1[0]=='#'){
				fprintf(stderr,"Error: wrong OUT_FOLDER parameter or empty\n");
				exit(-1);
			}
			folder_path=(char*)malloc(sizeof(char)*(strlen(buff1)+1));
			if(folder_path==NULL){
				fprintf(stderr,"Error: allocating string with folder path\n");
				exit(-1);
			}
			strcat(folder_path,buff1);
		}else if(!strcmp(buff1,"SPONGE_WIDTH")){
			fscanf(fd,"%d\n",&SPONGE_WIDTH);
			if(SPONGE_WIDTH<0){
				fprintf(stderr,"Error: SPONGE_WIDTH can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"N1")){
			fscanf(fd,"%d\n",&n1);
			if(n1<0){
				fprintf(stderr,"Error: N1 can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"N2")){
			fscanf(fd,"%d\n",&n2);
			if(n2<0){
				fprintf(stderr,"Error: N2 can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"N3")){
			fscanf(fd,"%d\n",&n3);
			if(n3<0){
				fprintf(stderr,"Error: N3 can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"NF")){
			fscanf(fd,"%d\n",&nf);
			if(nf<0){
				fprintf(stderr,"Error: NF can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"BC")){
			fscanf(fd,"%f\n",&bc_b);
			if(bc_b<0){
				fprintf(stderr,"Error: BC can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"DX")){
			fscanf(fd,"%f\n",&d1);
			d2=d1;
			d3=d1;
			if(d1<0){
				fprintf(stderr,"Error: DX can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"DT")){
			fscanf(fd,"%f\n",&dt);
			if(dt<0){
				fprintf(stderr,"Error: DT can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"JT")){
			fscanf(fd,"%d\n",&jt);
			if(jt<0){
				fprintf(stderr,"Error: JT can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"FREQ")){
			fscanf(fd,"%f\n",&fpeak);
			if(fpeak<0){
				fprintf(stderr,"Error: FREQ can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"SX")){
			fscanf(fd,"%d\n",&isx);
			if(isx<0){
				fprintf(stderr,"Error: SX can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"SY")){
			fscanf(fd,"%d\n",&isy);
			if(isy<0){
				fprintf(stderr,"Error: SY can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"SZ")){
			fscanf(fd,"%d\n",&isz);
			if(isz<0){
				fprintf(stderr,"Error: SZ can't be negative");
				exit(-1);
			}
		}else if(!strcmp(buff1,"RR")){
			fscanf(fd,"%s\n",buff1);
			if(!strcmp(buff1,"True"))
				rerun_receiver=1;
			else if(!strcmp(buff1,"False"))
				rerun_receiver=0;
			else{
				fprintf(stderr,"Error: invalid RR value");
				exit(-1);
			}
		}else if(!strcmp(buff1,"SUBS")){
			fscanf(fd,"%d\n",&num_subsurface_offsets);
			if(num_subsurface_offsets<0){
				fprintf(stderr,"Error: SUBS can't be negative");
				exit(-1);
			}
		}else
			continue;

		count++;
	}

	fclose(fd);

	if(count<15){
		fprintf(stderr,"Error: there are %d parameters out of 15!\n",count);
		fprintf(stderr,"\tList of parameters:\n");
		fprintf(stderr,"\tEMF: Name of the input earth model file (located in \"../tests/data\"\n");
		fprintf(stderr,"\tOUT_FOLDER: Name of the output folder\n");
		fprintf(stderr,"\tSPONGE_WIDTH: Boundaries around the image (points)\n");
		fprintf(stderr,"\tN1, N2 and N3: Sizes of the different dimensions 1->Z 2,3->X,Y (points)\n");
		fprintf(stderr,"\tNF: Number of frames\n");
		fprintf(stderr,"\tBC: Damping parameter\n");
		fprintf(stderr,"\tDX Sampling dimension in space (m/s)\n");
		fprintf(stderr,"\tDT: Sampling dimension in time (s)\n");
		fprintf(stderr,"\tJT: Propagation steps per frame of output\n");
		fprintf(stderr,"\tFREQ: Maximum frequency (Hz)\n");
		fprintf(stderr,"\tSX, SY, SZ: Source position (points)\n");
		fprintf(stderr,"\tRR: rerun receiver - if a temporary file with the receiver data exists but the user wants to run it again (True/False)\n");
		fprintf(stderr,"\tSUBS: Number of subsurface offsets\n");
		exit(-1);
	}

	if(folder_path==NULL){
		folder_path=(char*)malloc(sizeof(char)*(strlen("./tmp")+1));
		if(folder_path==NULL){
			fprintf(stderr,"Error: allocating string with folder path\n");
			exit(-1);
		}
		strcpy(folder_path,"./tmp");
	}

	BOUND_T=SPONGE_WIDTH;
	n123=(long long)n1*(long long)n2*(long long)n3;
	if(isx<0 || isx>n2){isx=n2/2;}
	if(isy<0 || isy>n3){isy=n3/2;}
	if(isz<0 || isz>n1){isz=SPONGE_WIDTH+1;}
	dscale = d1;
	nt = nf*jt;

	if(BOUND_T+SPONGE_WIDTH>=n3){fprintf(stderr,"WARNING: the boundaries are larger than n3\n");}
	if(2*SPONGE_WIDTH>=n2){fprintf(stderr,"WARNING: the boundaries are larger than n2\n");}
	if(2*SPONGE_WIDTH>=n1){fprintf(stderr,"WARNING: the boundaries are larger than n1\n");}

  printf("Parameters\n");
  printf("Input earth model: %s\n",(earth_model_file==NULL)?"(none), using water+salt simple model":earth_model_file);
  printf("Output folder: %s\n",folder_path);
  printf("Boundary: T=%d, E=%d (points)\n",BOUND_T,SPONGE_WIDTH);
  printf("Dimensions: n1=%d, n2=%d, n3=%d (points)\n",n1,n2,n3);
  printf("Number of frames nf=%d\n",nf);
  printf("Damping: bc=%f\n",bc_b);
  printf("Sampling: dx=%f (m)\n",d1);
  printf("Time scale dt=%f (s)\n",dt);
  printf("Time steps per frame: jt=%d\n",jt);
  printf("Number of time steps: nt=%d\n",nt);
  printf("Frequency: fpeak=%f (Hz)\n",fpeak);
  printf("Source: (x,y,z) = (%d,%d,%d)\n",isx,isy,isz);
  printf("Rerun receiver: %s\n",(rerun_receiver)?"True":"False");
  printf("Subsurface offsets: %d\n",num_subsurface_offsets);

//printf(stderr,"\33[34m Parameters\n");
//printf(stderr,"\33[34m Input earth model: %s\n",(earth_model_file==NULL)?"(none), using water+salt simple model":earth_model_file);
//printf(stderr,"\33[34m Output folder: %s\n",folder_path);
//printf(stderr,"\33[34m Boundary: T=%d, E=%d (points)\n",BOUND_T,SPONGE_WIDTH);
//printf(stderr,"\33[34m Dimensions: n1=%d, n2=%d, n3=%d (points)\n",n1,n2,n3);
//printf(stderr,"\33[34m Number of frames nf=%d\n",nf);
//printf(stderr,"\33[34m Damping: bc=%f\n",bc_b);
//printf(stderr,"\33[34m Sampling: dx=%f (m)\n",d1);
//printf(stderr,"\33[34m Time scale dt=%f (s)\n",dt);
//printf(stderr,"\33[34m Time steps per frame: jt=%d\n",jt);
//printf(stderr,"\33[34m Number of time steps: nt=%d\n",nt);
//printf(stderr,"\33[34m Frequency: fpeak=%f (Hz)\n",fpeak);
//printf(stderr,"\33[34m Source: (x,y,z) = (%d,%d,%d)\n\33[30m",isx,isy,isz);
//printf(stderr,"\33[34m Rerun receiver: %s\n\33[30m",(rerun_receiver)?"True":"False");
//printf(stderr,"\33[34m Subsurface offsets: %d\n\33[30m",num_subsurface_offsets);

	return earth_model_file;
}
//--------------------------------------------------------------
// Create and initialise fields and image
//--------------------------------------------------------------
float *pon_s;
float *pc_s;
float *pc_g;
float *pon_g; //Wavefield fields
float *source; //Image
float **image; //Image
void create_fields(){
	int sub,i;

	pon_s=(float*)malloc(sizeof(float)*n123);
	if(pon_s==NULL){
		fprintf(stderr,"Error: couldn't properly allocate pon_s buffer!\n");
		exit(-1);
	}

	pc_s =(float*)malloc(sizeof(float)*n123);
	if(pc_s==NULL){
		free(pon_s);
		fprintf(stderr,"Error: couldn't properly allocate pc_s buffer!\n");
		exit(-1);
	}

	pon_g=(float*)malloc(sizeof(float)*n123);
	if(pon_g==NULL){
		free(pon_s);
		free(pc_s);
		fprintf(stderr,"Error: couldn't properly allocate pon_g buffer!\n");
		exit(-1);
	}

	pc_g =(float*)malloc(sizeof(float)*n123);
	if(pc_g==NULL){
		free(pon_s);
		free(pc_s);
		free(pon_g);
		fprintf(stderr,"Error: couldn't properly allocate pc_g buffer!\n");
		exit(-1);
	}

	source=(float*)malloc(sizeof(float)*n123);
	if(source==NULL){
		free(pon_s);
		free(pc_s);
		free(pon_g);
		free(pc_g);
		fprintf(stderr,"Error: couldn't properly allocate source buffer!\n");
		exit(-1);
	}

	//array of images
	image=(float**)malloc(sizeof(float*)*(num_subsurface_offsets+1));
	if(image==NULL){
		free(pon_s);
		free(pc_s);
		free(pon_g);
		free(pc_g);
		free(source);
		fprintf(stderr,"Error: couldn't properly allocate image buffer!\n");
		exit(-1);
	}

	//create one image for each subsurface offset (including offset 0)
	for(sub=0; sub<=num_subsurface_offsets; sub++){
		image[sub]=(float*)malloc(sizeof(float)*n123);
		if(image[sub]==NULL){
			free(pon_s);
			free(pc_s);
			free(pon_g);
			free(pc_g);
			free(source);
			fprintf(stderr,"Error: couldn't properly allocate image buffer!\n");
			exit(-1);
		}
		// Init
#pragma omp parallel for
		for(i=0; i < n123; i++) {image[sub][i]=0.0;}
	}

	// Init
#pragma omp parallel for
	for(i=0; i < n123; i++) {
		source[i]=0.0;
		pon_s[i]=0.0;
		pon_g[i]=0.0;
		pc_s[i]=0.0;
		pc_g[i]=0.0;
	}

#ifdef DEBUG
	fprintf(stderr,"FINISHED CREATING FIELDS\n");
#endif
	return;
}
void clean_receiver_field(){
	int i;

#pragma omp parallel for
	for(i=0; i < n123; i++) {
		pon_g[i]=0.0;
		pc_g[i]=0.0;
	}
	return;
}
void clean_source(){
	int i;

#pragma omp parallel for
	for(i=0; i < n123; i++) {source[i]=0.0;}
	return;
}
// Free field arrays
void clean_fields(){
	int i;

	if(pon_s)free(pon_s);
	if(pc_s)free(pc_s);
	if(pon_g)free(pon_g);
	if(pc_g)free(pc_g);
	if(source)free(source);
	for(i=0; i<=num_subsurface_offsets; i++){if(image[i])free(image[i]);}
	if(image)free(image);
	return;
}
//--------------------------------------------------------------
// Data
//--------------------------------------------------------------
float *data;  //Data
void create_data(){
	data =(float*)malloc(sizeof(float)*n2*n3*nf); //Record data at whole surface for now
	if(data==NULL){
		clean_fields();
		fprintf(stderr,"Error: couldn't properly allocate Data buffer!\n");
		exit(-1);
	}
}
// Free Data
void clean_data(){
	if(data)free(data);
	return;
}
int get_receiver_from_file(char *file_name){
	int return_value;
	FILE* receiver_file;

	return_value = 0;

	receiver_file = fopen(file_name, "rb");
	if (receiver_file == NULL){
		printf("Receiver file not found: propagating receiver to gather data!\n");
		return_value = 1;
	}else{

		if(fread(data, sizeof(float), n2*n3*nf, receiver_file) != n2*n3*nf) {
			fprintf(stderr,"Error: while reading receiver file '%s' !\n",file_name);
			return_value = 1;
		}
		fclose(receiver_file);
		printf("Found receiver file: skipping receiver propagation!\n");
	}
	return return_value;
}
void save_receiver_to_file(char *file_name){
	int num;
	FILE* receiver_file = fopen(file_name, "w+");
	if (receiver_file == NULL){
		printf("Error: while opening file to dump the receiver data!\n");
		clean_fields();
		clean_data();
		exit(-1);
	}

	num = fwrite(data, sizeof(float), n3*n2*nf, receiver_file);
	if(num!=nf*n2*n3){
		clean_fields();
		clean_data();
		fprintf(stderr,"Error: couldn't write the whole receiver data!\n");
		exit(-1);
	}
	fclose(receiver_file);
	return;
}
//--------------------------------------------------------------
// Read earth model from file
//--------------------------------------------------------------
float *vel_s;
float *vel_g;  //Velocity with and without random bounds
void create_earth_model_from_file(const char *fname)
{
	FILE *fd;

	vel_s=(float*)malloc(sizeof(float)*n123);
	if(vel_s==NULL){
		clean_fields();
		clean_data();
		fprintf(stderr,"Error: couldn't properly allocate velocity buffer!\n");
		exit(-1);
	}

	vel_g=(float*)malloc(sizeof(float)*n123);
	if(vel_g==NULL){
		fprintf(stderr,"Error: couldn't properly allocate velocity bounds buffer!\n");
		clean_fields();
		clean_data();
		free(vel_s);
		exit(-1);
	}

	fd = fopen(fname, "rb");
	if (fd == NULL) {
		fprintf(stderr,"Error: invalid earth model file '%s' !\n",fname);
		clean_fields();
		clean_data();
		free(vel_s);
		free(vel_g);
		exit(-1);
	}

	if(fread(vel_g, sizeof(float), n123, fd) != n123) {
		fprintf(stderr,"Error: while reading earth model file '%s' !\n",fname);
		clean_fields();
		clean_data();
		free(vel_s);
		free(vel_g);
		exit(-1);
	}

	fclose(fd);
	return;
}
int read_key_value(FILE *fd, char *key, char *value)
{
	char string[512];

	int n = fscanf(fd, "%s", string);

	if (n > 0) {
		char *p[2];
		p[0] = strtok(string, "=");
		p[1] = strtok(NULL, "=");
		p[1] = strtok(p[1], "\"");
		strcpy(key, p[0]);
		strcpy(value, p[1]);
	}

	return n;
}
void read_earth_model(char *filename)
{
	int i;
	char key[256];
	char value[256];
	char filepath[256];
	int aux;
	float aux_f;
	FILE *fd;

	fd = fopen(filename, "r");
	if (fd == NULL) {
		fprintf(stderr,"Error: invalid parameter earth model file '%s' !\n",filename);
		clean_fields();
		clean_data();
		exit(-1);
	}

	while(read_key_value(fd,key,value)!=EOF){
		if(key[0]=='#')continue;

		if (!strcmp(key, "in")) {
			strcpy(filepath, filename);
			char *p = strrchr(filepath, '/');
			p[1] = '\0';
			strcat(filepath,value);
		} else if (strcmp(key, "n1") == 0) {
			aux = atoi(value);
			if(aux != n1){
				fprintf(stderr, "ERROR: Z dimension in earth model file %d does not agree with parameter file %d, dimensions must agree\n",aux,n1);
				clean_fields();
				clean_data();
				fclose(fd);
				exit(-1);
			}
		} else if (strcmp(key, "n2") == 0) {
			aux = atoi(value);
			if(aux != n2){
				fprintf(stderr, "ERROR: X dimension in earth model file %d does not agree with parameter file %d, dimensions must agree\n",aux,n2);
				clean_fields();
				clean_data();
				fclose(fd);
				exit(-1);
			}
		} else if (strcmp(key, "n3") == 0) {
			aux = atoi(value);
			if(aux != n3){
				fprintf(stderr, "ERROR: Y dimension in earth model file %d does not agree with parameter file %d, dimensions must agree\n",aux,n3);
				clean_fields();
				clean_data();
				fclose(fd);
				exit(-1);
			}
		} else if (strcmp(key, "dscale") == 0) {
			aux_f = (float) 1/atof(value);//points/meter
			//if(aux_f-d1 > 0.001) fprintf(stderr, "WARNING: D1 in earth model file %f does not agree with parameter file DSCALE %f\n",d1,aux_f);
			//if(aux_f-d2 > 0.001) fprintf(stderr, "WARNING: D2 in earth model file %f does not agree with parameter file DSCALE %f\n",d2,aux_f);
			//if(aux_f-d3 > 0.001) fprintf(stderr, "WARNING: D3 in earth model file %f does not agree with parameter file DSCALE %f\n",d3,aux_f);
			//dscale = aux_f;
		} else if (strcmp(key, "dt") == 0) {
			aux_f = (float) atof(value);//seconds/time step
			//if(aux_f-dt > 0.001) fprintf(stderr, "WARNING: DT in earth model file %f does not agree with parameter file, keeping %f\n",aux_f,dt);
		}
	}

	fclose(fd);

	create_earth_model_from_file(filepath);

	//Find max and min velocity
	vmax = vel_g[0];
	vmin = vel_g[0];
	for(i=1; i < n123; i++){
		vmax=(vel_g[i]>vmax)?vel_g[i]:vmax;
		vmin=(vel_g[i]<vmin)?vel_g[i]:vmin;
	}

#ifdef DEBUG
	fprintf(stderr,"FINISHED CREATING  MODEL FROM FILE\n");
#endif
	return;
}
//--------------------------------------------------------------
// Simple velocity model
//--------------------------------------------------------------
// Construct velocity model if we are not reading the model from a file
void create_vel(){
	long long i;
	long long i2;
	int borderA;

	borderA = n1*0.5;

	vel_s=(float*)malloc(sizeof(float)*n123);
	if(vel_s==NULL){
		clean_fields();
		clean_data();
		fprintf(stderr,"Error: couldn't properly allocate velocity buffer!\n");
		exit(-1);
	}

	vel_g=(float*)malloc(sizeof(float)*n123);
	if(vel_g==NULL){
		clean_fields();
		clean_data();
		free(vel_s);
		fprintf(stderr,"Error: couldn't properly allocate velocity bounds buffer!\n");
		exit(-1);
	}

#pragma omp parallel for default(shared) private(i)
	for(i=0; i < n123; i++) vel_g[i]=1500.; //water

#pragma omp parallel for
    for(i2=0; i2 < n2*n3; i2++){
		int i;
		for(i=borderA; i<n1; i++)
			vel_g[i+n1*i2]=4500.;//salt
	}

	for(i=0; i < n123; i++) vel_s[i]=vel_g[i]; //init vel_s

	//Find max and min velocity
	vmax = vel_g[0];
	vmin = vel_g[0];
	for(i=1; i < n123; i++){
		vmax=(vel_g[i]>vmax)?vel_g[i]:vmax;
		vmin=(vel_g[i]<vmin)?vel_g[i]:vmin;
	}

#ifdef DEBUG
	fprintf(stderr,"FINISHED CREATING SIMPLE VELOCITY MODEL\n");
#endif
	return;
}
// Free velocity arrays
void clean_vel(){
	if(vel_s)free(vel_s);
	if(vel_g)free(vel_g);
	return;
}
//--------------------------------------------------------------
// Damping region
//--------------------------------------------------------------
float *bound_e;  //Boundary arrays
// Construct damping region
void construct_bound(){
	int i1;

	bound_e=(float*)malloc(sizeof(float)*TOTAL_SPONGE_WIDTH);
	if(bound_e==NULL){
		clean_fields();
		clean_data();
		clean_vel();
		fprintf(stderr,"Error: couldn't properly allocate boundary array!\n");
		exit(-1);
	}

#pragma omp parallel for
	for(i1=0; i1 < TOTAL_SPONGE_WIDTH; i1++){
		if(i1<SPONGE_WIDTH){
			bound_e[i1]=expf(-bc_b *(SPONGE_WIDTH-i1)); // exponential decay
		}else{
			bound_e[i1]=1.;
		}
	}
#ifdef DEBUG
	fprintf(stderr,"FINISHED CONSTRUCTING BOUNDARIES\n");
#endif
	return;
}
// Free bound array
void clean_bound(){
	if(bound_e)free(bound_e);
	return;
}
//--------------------------------------------------------------
// Construct random boundary and square velocity
//--------------------------------------------------------------
void random_bound_dvv(){
	// Random seed

	int i;
	int i3;

	srand(irand); //Initialise random number generator

#pragma omp parallel for
	for(i3=0; i3 < n3; i3++){  //Loop over slowest
		int i1;
		int i2;
		float rnd;

		float f3=0;
		// Pow of normalised distance to the border 3 f3=((x-n3)/n3)^2
		if(i3 >=n3-SPONGE_WIDTH)f3=(float)(i3-(n3-SPONGE_WIDTH-1))*(i3-(n3-SPONGE_WIDTH-1))/(float)(SPONGE_WIDTH*SPONGE_WIDTH);
		if(i3<SPONGE_WIDTH)f3=(float)((SPONGE_WIDTH-i3)*(SPONGE_WIDTH-i3))/(float)(SPONGE_WIDTH*SPONGE_WIDTH);

		//Fractional distance within boundary axis 3

		for(i2=0; i2 < n2; i2++){ //Loop over middle
			float f2=0;
			// Pow of normalised distance to the border 1 f2=((x-n2)/n2)^2
			if(i2>=n2-SPONGE_WIDTH)f2=(float)(i2-(n2-SPONGE_WIDTH-1))*(i2-(n2-SPONGE_WIDTH-1))/(float)(SPONGE_WIDTH*SPONGE_WIDTH);
			if(i2<SPONGE_WIDTH)f2=(float)((SPONGE_WIDTH-i2)*(SPONGE_WIDTH-i2))/(float)(SPONGE_WIDTH*SPONGE_WIDTH);
			//Fractional distance within boundary axis 3

			for(i1=0; i1 < n1; i1++){ //Loop over fastest
				float f1=0;
				float dev=0.0;
				// Pow of normalised distance to the border 1 f1=((x-n1)/n1)^2
				if(i1>=n1-SPONGE_WIDTH)f1=(float)(i1-(n1-SPONGE_WIDTH-1))*(i1-(n1-SPONGE_WIDTH-1))/(float)(SPONGE_WIDTH*SPONGE_WIDTH);
				if(i1<BOUND_T)f1=(float)((BOUND_T-i1)*(BOUND_T-i1))/(float)(BOUND_T*BOUND_T);

				// Normalised distance to the border sqrt(f1+f2+f3)
				float dist=sqrtf(f1+f2+f3); //Distance within the boundary

				// If the distance is positive then find a proper random number
				int found=(dist<.0001) ? 1 : 0;
				float val=vel_g[i3*n2*n1+i2*n1+i1];
				while(found==0){
					rnd=(float)rand()/RAND_MAX-.5-.49*dist/SPONGE_WIDTH;  //Choose random number
					dev=rnd*2.5*dist*val;
					// Accept the number if it is smaller than the maximum velocity and positive
					if(fabs(dev+val)< vmax*1.01 && dev+val > 0.0001) found=1;
				}

				vel_g[i3*n2*n1+i2*n1+i1]=val;        //Receiver velocity
				vel_s[i3*n2*n1+i2*n1+i1]=(val+dev);  //Source velocity with randomness
			}
		}
	}

#ifdef DEBUG
	fprintf(stderr,"FINISHED CREATING RANDOM BOUNDARIES\n");
#endif
	return;
}
//--------------------------------------------------------------
// Square earth model dvv
//--------------------------------------------------------------
void square_earth_model(){
	int i;

	float velmax_s = vmax;
	float velmin_s = vmin;

	float dvvmax = 0.0f;
	float dvvmin = 1.0e20f;

	for(i = 0; i < n123; i++) {
		if(vel_s[i] > velmax_s) velmax_s = vel_s[i];
		if(vel_s[i] < velmin_s) velmin_s = vel_s[i];

		vel_g[i] = vel_g[i]*vel_g[i];
		vel_s[i] = vel_s[i]*vel_s[i];

		if(vel_s[i] > dvvmax) dvvmax = vel_s[i];
		if(vel_s[i] < dvvmin) dvvmin = vel_s[i];
		if(vel_g[i] > dvvmax) dvvmax = vel_g[i];
		if(vel_g[i] < dvvmin) dvvmin = vel_g[i];
	}

	if(dt > d1/(2.26*velmax_s)){
		fprintf(stderr,"WARNING: This model may become unstable. The relation maxV*(DT/DX)<1/2.26 is not met for maxV=%f m/s\n",velmax_s);
		fprintf(stderr,"         Change the parameters file by increasing the grid size (DX) or reducing the timing step (DT) to meet the above relation\n");
	}

#ifdef DEBUG
	fprintf(stderr,"  velmax = %g\n", velmax_s);
	fprintf(stderr,"  velmin = %g\n", velmin_s);
	fprintf(stderr,"  dvvmax = %g\n", dvvmax);
	fprintf(stderr,"  dvvmin = %g\n", dvvmin);
#endif
}
//--------------------------------------------------------------
// Dump Image to File
//--------------------------------------------------------------
void create_header_file(char *file_name)
{
	char *header_file_name = (char*)malloc((strlen(file_name)+3)*sizeof(char));

	strcpy(header_file_name, file_name);
	strcat(header_file_name, ".H");
	FILE* header_file = fopen(header_file_name, "w");
	if (header_file == NULL)
	{
		fprintf(stderr,"Error creating header file: %s.", header_file_name);
		clean_fields();
		clean_data();
		clean_vel();
		clean_bound();
		exit(-1);
	}

	char *file_name_stripped = strrchr(file_name,'/');
	if (file_name_stripped == NULL){
		fprintf(header_file, "in=%s\n", file_name);
	}else{
		fprintf(header_file, "in=%s\n", file_name_stripped);
	}
	fprintf(header_file, "n1=%d\n", n1);
	fprintf(header_file, "n2=%d\n", n2);
	fprintf(header_file, "n3=%d\n", n3);

	fprintf(header_file, "esize=1\n");

	fclose(header_file);

	free(header_file_name);
	return;
}
void dump_image_to_file(char *name){
	FILE *fd;
	int num, i;
	char tmp_name[64];

	for(i=0; i<=num_subsurface_offsets; i++){
		strcpy(tmp_name,name);
		sprintf(tmp_name,"%s%d",tmp_name,i);

		fd = fopen(tmp_name,"w+");
		if(fd == NULL){
			clean_fields();
			clean_data();
			clean_vel();
			clean_bound();
			fprintf(stderr,"Error: can't open output file!\n");
			exit(-1);
		}

		num = fwrite(image[i], sizeof(float), n1*n2*n3, fd);
		if(num!=n1*n2*n3){
			clean_fields();
			clean_data();
			clean_vel();
			clean_bound();
			fprintf(stderr,"Error: couldn't write the whole image!\n");
			exit(-1);
		}

		fprintf(stderr,"IMAGE %d STORED IN %s\n",i,tmp_name);

		fclose(fd);
	}

	create_header_file(name);
	return;
}
