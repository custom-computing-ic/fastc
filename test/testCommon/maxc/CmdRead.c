#include "../../../include/maxcc.h"

#ifndef Burst_inc
#define Burst_inc 1
#endif

void kernel_CmdRead(
                    unsigned int iniBursts,
                    unsigned int totalBursts,
                    unsigned int wordsPerBurst,
                    bool Enable)
{
  int32 wordCount = count_p(32, wordsPerBurst, 1, Enable);

  int32 wrap = (wordCount == wordsPerBurst - 1) & Enable;
  int32 burstCount = count_p(32, totalBursts, Burst_inc, wrap);

  int32 Control = (wordCount == 0) & Enable;

  DRAMOutput("dram_read",
             Control,
             burstCount + iniBursts,
             Burst_inc,
             1,
             0,
             0);
}
