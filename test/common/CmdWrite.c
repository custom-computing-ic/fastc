#include "maxcc.h"

#ifndef Burst_inc
#define Burst_inc 1
#endif

void kernel_CmdWrite(unsigned int iniBursts,
                     unsigned int iterations,
                     unsigned int totalBursts,
                     unsigned int wordsPerBurst,
                     bool Enable) {

  int wordCount  = count(32, wordsPerBurst, 1, Enable);
  int burstCount = count(32, totalBursts, Burst_inc, wordCount);
  int iterCount = count(32, iterations, 1, burstCount);

  bool Control = (wordCount == 0) & Enable;
  DRAMOutput("dram_write",
             Control,
             burstCount + iniBursts,
             Burst_inc,
             1,
             0,
             (burstCount == totalBursts - Burst_inc) && iterCount == iterations - 1);
}
