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
  int wordCount = count(32, wordsPerBurst, 1, Enable);
  int burstCount = count(32, totalBursts, Burst_inc, wordCount);

  bool Control = (wordCount == 0) & Enable;

  DRAMOutput("dram_read",
             Control,
             burstCount + iniBursts,
             Burst_inc,
             1,
             0,
             0);
}
