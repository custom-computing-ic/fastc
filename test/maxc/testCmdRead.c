#define burst_inc 1

#pragma scalar in iniBursts uint32 kernel_Cmdread
#pragma scalar in totalBursts uint32 kernel_Cmdread
#pragma scalar in wordsPerBurst uint32 kernel_Cmdread
#pragma scalar in Enable uint1 kernel_Cmdread
void kernel_Cmdread(
    int iniBursts,
    int totalBursts,
    int wordsPerBurst,
    int Enable)
{
    int wordCount  = count_p(32, wordsPerBurst, 1, Enable);

    int wrap = (wordCount == wordsPerBurst);
    int burstCount = count_p(32, totalBursts, burst_inc, wrap);

    int Control = (wordCount == 0) & Enable;

    DRAMOutput("dram_read",
               Control,
               burstCount + iniBursts,
               burst_inc,
               1,
               0,
               0);
}
