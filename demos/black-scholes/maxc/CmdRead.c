#include "../../../include/maxcc.h"

#ifndef Burst_inc
#define Burst_inc 1
#endif

void kernel_Cmdread(
		    unsigned int iniBursts,
		    unsigned int totalBursts,
		    unsigned int wordsPerBurst,
		    bool Enable)
{
    int wordCount = count_p(32, wordsPerBurst, 1, Enable);

    int* wrap;
    *wrap = (wordCount == wordsPerBurst - 1) & Enable;
    int burstCount = count_p(32, totalBursts, Burst_inc, wrap);

    int *Control;
    *Control = (wordCount == 0) & Enable;

    DRAMOutput("dram_read",
               Control,
               burstCount + iniBursts,
               Burst_inc,
               1,
               0,
               0);
}
