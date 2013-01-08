#define burst_inc 1

#pragma scalar in iniBursts uint32 kernel_Cmdwrite
#pragma scalar in iterations uint32 kernel_Cmdwrite
#pragma scalar in totalBursts uint32 kernel_Cmdwrite
#pragma scalar in wordsPerBurst uint32 kernel_Cmdwrite
#pragma scalar in Enable uint1 kernel_Cmdwrite
void kernel_Cmdwrite(int iniBursts,
                         int iterations,
                         int totalBursts,
                         int wordsPerBurst,
                         int Enable) {

int wordCount  = count_p(32, wordsPerBurst, 1, Enable);

int wrap = (wordCount == wordsPerBurst);
int burstCount = count_p(32, totalBursts, 1, wrap);

int wrap2 = (burstCount == totalBursts);
int iterCount = count_p(32, iterations, 1, wrap2);

int Control = (wordCount == 0) & Enable;
DRAMOutput("dram_write",
               Control,
               burstCount + iniBursts,
               burst_inc,
               1,
               0,
               (burstCount == totalBursts - burst_inc) && iterCount == iterations - 1);
}
