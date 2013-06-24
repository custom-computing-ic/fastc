#ifndef __BBHSTDIO_H__
#define __BBHSTDIO_H__
#include <stdint.h>
#define BBHPUTCHAR 0x1
#define BBHPUTS 0x2
#define BBHPRINTF 0x3
extern int bbhputchar(int c);
extern int bbhputs(char* c);
extern int bbhprintf(const char* str,...);
extern uint32_t pe_system_call(uint32_t code);

#endif
