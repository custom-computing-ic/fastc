typedef int* s_int32;
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
    s_int32 wordCount;
    wordCount[0] = count_p(32, wordsPerBurst, 1, Enable);

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
