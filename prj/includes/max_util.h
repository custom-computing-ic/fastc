/* max_util.h ---- Brittle 2011 */

#ifndef __MAX_UTIL_H__
#define __MAX_UTIL_H__

#include <MaxCompilerRT.h>
int MAX_Conf_CustomAdd(char *stream_name, unsigned offset_bursts, int data_size, int Par, long long *address_stream, int write );
int MAX_CustomAdd(uint32_t *data, unsigned size_b, char *address);  
int MAX_Add(int start_address, int data_size, long long *address_stream);


int MAX_Init(char *device_name);
void MAX_Finish(void);

int MAX_Conf_Mem1D_GBSL (
    char *stream_name,
    char *group_name,
    unsigned offset_bytes,
    unsigned bx_bytes,
    unsigned sx_bytes,
    unsigned lx_bytes);
int MAX_Conf_Mem1D_G (
    char *stream_name,
    char *group_name,
    unsigned offset_bytes,
    unsigned data_length_bytes,
    unsigned iterations);
int MAX_Conf_Mem1D (
    char *stream_name,
    unsigned offset_bytes,
    unsigned data_length_bytes,
    unsigned iterations);
int MAX_Enable_Mem(char *stream_name);
int MAX_Disable_Mem(char *stream_name);
int MAX_PCI2Mem(uint32_t *data, unsigned size_b, char *stream_pci, char *stream_mem);
int MAX_Mem2PCI(void *data, int size_b, char *stream_pci, char *stream_mem);

void MAX_Reset(void);
int MAX_Set_Int(char *stream_name);
void MAX_Wait_Int(void);
void MAX_Set_Cycles(char *kernel_name, int size);
void MAX_Set_FScalar(char *scalar_symbol, float value);

#endif
