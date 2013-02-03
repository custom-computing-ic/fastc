#ifndef MAXCC_H
#define MAXCC_H


// simple types
typedef float float8_24;
typedef int int32;
typedef int uint32;

// stream types
typedef float* s_float8_24;
typedef int* s_uint32_in;
typedef int* s_int32;
typedef int* s_uint32;

// array types
typedef s_float8_24* s_array_f8_24;

// offset expressions
typedef int s_offset;

// output functions
void output_i(int* res, int func);

void output_ic(int* res, int func, int32 cond);

void output_iaf(s_array_f8_24 dest, s_array_f8_24 src, int mantissa, int exponent, int width);

void DRAMOutput(char *stream_name, int Control,
		int address, int burstCount, int burstInc,
		int stream_num , int tag);

// input functions
s_array_f8_24 make_input_array_f(int mantissa, int exponent, int width);

// counter functions
int count(int a, int b);
int count_chain(int a, int b, int parent);
int count_p(int width, int wrapPoint, int inc, int enable);

// offset functions
int make_offset(int min, int max);

// control/multiplexer functions
int fselect(float8_24 cond, float8_24 left, float8_24 right);

int fselect_sf_f(float8_24 cond, s_float8_24 left, float8_24 right);

// array functions
s_array_f8_24 make_array_f(int mantissa, int exponent, int width);

// optimization functions
void pushDSPFactor(float factor);

#endif
