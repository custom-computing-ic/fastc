#include "../../../include/maxcc.h"

#ifndef Burst_inc
#define Burst_inc 1
#endif

void kernel_Cmdwrite(unsigned int iniBursts,
                         unsigned int iterations,
                         unsigned int totalBursts,
                         unsigned int wordsPerBurst,
                         bool Enable) {

    int wordCount  = count_p(32, wordsPerBurst, 1, Enable);

    int* wrap, *wrap2, *Control;
    *wrap = (wordCount == wordsPerBurst - 1) & Enable;
    int burstCount = count_p(32, totalBursts, Burst_inc, wrap);

    *wrap2 = (burstCount == totalBursts - Burst_inc) & *wrap;
    int iterCount = count_p(32, iterations, 1, *wrap2);

    *Control = (wordCount == 0) & Enable;
    DRAMOutput("dram_write",
	       Control,
               burstCount + iniBursts,
               Burst_inc,
               1,
               0,
               (burstCount == totalBursts - Burst_inc) && iterCount == iterations - 1);
}
