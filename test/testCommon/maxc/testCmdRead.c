#include "../../../include/maxcc.h"

#define burst_inc 1

#pragma class:scalar dir:in name:iniBursts type:uint32 func:kernel_Cmdread
#pragma class:scalar dir:in name:totalBursts type:uint32 func:kernel_Cmdread
#pragma class:scalar dir:in name:wordsPerBurst type:uint32 func:kernel_Cmdread
#pragma class:scalar dir:in name:Enable type:uint1 func:kernel_Cmdread
void kernel_Cmdread(
    int iniBursts,
    int totalBursts,
    int wordsPerBurst,
    int Enable)
{
    int32 wordCount = count_p(32, wordsPerBurst, 1, Enable);

    int32 wrap = (wordCount == wordsPerBurst);
    int32 burstCount = count_p(32, totalBursts, burst_inc, wrap);

    int32 Control = (wordCount == 0) & Enable;

    DRAMOutput("dram_read",
               Control,
               burstCount + iniBursts,
               burst_inc,
               1,
               0,
               0);
}
