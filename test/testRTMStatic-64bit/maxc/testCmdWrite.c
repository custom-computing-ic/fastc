#include "../../../include/maxcc.h"

#define burst_inc 1


#pragma class:scalar dir:in name:iniBursts type:uint32 func:kernel_Cmdwrite
#pragma class:scalar dir:in name:iterations type:uint32 func:kernel_Cmdwrite
#pragma class:scalar dir:in name:totalBursts type:uint32 func:kernel_Cmdwrite
#pragma class:scalar dir:in name:wordsPerBurst type:uint32 func:kernel_Cmdwrite
#pragma class:scalar dir:in name:Enable type:uint1 func:kernel_Cmdwrite
void kernel_Cmdwrite(int iniBursts,
                         int iterations,
                         int totalBursts,
                         int wordsPerBurst,
                         int Enable) {

    int32 wordCount  = count_p(32, wordsPerBurst, 1, Enable);

    int32 wrap = (wordCount == wordsPerBurst - 1) & Enable;
    int32 burstCount = count_p(32, totalBursts, 1, wrap);

    int32 wrap2 = (burstCount == totalBursts - 1) & wrap;
    int32 iterCount = count_p(32, iterations, 1, wrap2);

    int32 Control = (wordCount == 0) & Enable;
    DRAMOutput("dram_write",
	       Control,
               burstCount + iniBursts,
               burst_inc,
               1,
               0,
               (burstCount == totalBursts - burst_inc) && iterCount == iterations - 1);
}
