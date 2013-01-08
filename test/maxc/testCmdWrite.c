#define burst_inc 1

#pragma scalar in iniBursts uint32 kernel_Cmdwrite
#pragma scalar in totalBursts uint32 kernel_Cmdwrite
#pragma scalar in wordsPerBurst uint32 kernel_Cmdwrite
#pragma scalar in Enable uint32 kernel_Cmdwrite
void kernel_Cmdwrite(
                     int iniBursts, int totalBursts, int wordsPerBurst,
                     int Enable) {

    int wordCount  = count_p(32, wordsPerBurst, 1, Enable);

    int wrap = (wordCount == wordsPerBurst);
    int burstCount = count_p(32, burst_inc, 1, wrap);

    int Control = (wordCount == 0) & Enable;

    DRAMOutput("dram_read",
               Control,
               burstCount + iniBursts,
               burst_inc,
               1,
               0,
               0);
}
