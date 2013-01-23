#pragma dir:in name:p type:float8_24 func:kernel_Convolution1d
#pragma class:scalar dir:in name:c_0_0_0 type:float8_24 func:kernel_Convolution1d
#pragma class:scalar dir:in name:c_p_0_0 type:float8_24 func:kernel_Convolution1d
#pragma class:scalar dir:in name:c_n_0_0 type:float8_24 func:kernel_Convolution1d
#pragma class:scalar dir:in name:n1 type:uint32 func:kernel_Convolution1d
#pragma class:scalar dir:in name:ORDER type:uint32 func:kernel_Convolution1d
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
