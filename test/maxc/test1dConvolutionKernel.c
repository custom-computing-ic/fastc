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
