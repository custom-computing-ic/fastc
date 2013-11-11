#include "test1dConvolutionKernel.c"

#include "../../testCommon/maxc/testCmdRead.c"

#include "../../testCommon/maxc/testCmdWrite.c"

/*
#pragma in p CPU kernel_Convolution1d
#pragma scalar in c_0_0_0 float8_24 kernel_Convolution1d
#pragma scalar in c_p_0_0 float8_24 kernel_Convolution1d
#pragma scalar in c_n_0_0 float8_24 kernel_Convolution1d
#pragma scalar in n1 uint32 kernel_Convolution1d
#pragma scalar in ORDER uint32 kernel_Convolution1d
void kernel_Convolution1d(
                          float *p,
                          float *output,
                          float c_0_0_0,
                          float c_p_0_0,
                          float c_n_0_0,
                          int n1,
                          int ORDER) {
    int i4 = count(1000, 1);
    int i1 = countChain(n1, 1, i4);

    int up = (i1 >= ORDER) && (i1 < n1 - ORDER);

    pushDSPFactor(1);
    int result =
        p[0]  * c_0_0_0 +
        p[1]  * c_p_0_0 +
	p[-1] * c_n_0_0;

    int inter = fselect(up, p, result);
    popDSPFactor();

    output_i(output, inter);
    }

#pragma class:scalar in iniBursts uint32 kernel:kernel_Cmdwrite
#pragma scalar in iterations uint32 kernel:kernel_Cmdwrite
#pragma scalar in totalBursts uint32 kernel:kernel_Cmdwrite
#pragma scalar in wordsPerBurst uint32 kernel:kernel_Cmdwrite
#pragma scalar in Enable uint1 kernel_Cmdwrite
void kernel_Cmdwrite(int iniBursts,
                     int iterations,
                     int totalBursts,
                     int wordsPerBurst,
                     int Enable) {

    int wordCount  = count_p(32, wordsPerBurst, 1, Enable);

    // int wrap = (wordCount == wordsPerBurst );
    int wrap = (wordCount == 0);
    int burstCount = count_p(32, totalBursts, 1, wrap);

    //int wrap2 = (burstCount == totalBursts);
    int wrap2 = (burstCount == 0);
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

    //    int wrap = (wordCount == wordsPerBurst);
    int wrap = (wordCount == 0);
    int burstCount = count_p(32, totalBursts, burst_inc, wrap);

    int Control = (wordCount == 0) & Enable;

    DRAMOutput("dram_read",
               Control,
               burstCount + iniBursts,
               burst_inc, 1, 0, 0);
}
*/
