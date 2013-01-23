typedef int* s_int32;

#pragma dir:in name:x type:float8_24 func:kernel_KernelFunc
#pragma dir:out name:res type:float8_24 func:kernel_KernelFunc
#pragma class:scalar dir:in name:c type:float8_24 func:kernel_KernelFunc
#pragma class:scalar dir:in name:a1 type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:a2 type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:a3 type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:n1 type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:n2 type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:n3 type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:nx type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:ny type:uint32 func:kernel_KernelFunc
#pragma class:scalar dir:in name:nz type:uint32 func:kernel_KernelFunc
void kernel_KernelFunc(int *x, int *res,
                       int nx, int ny, int nz,
                       int a1, int a2, int a3,
                       int n1, int n2, int n3,
                       int c) {

    // counters are required to keep track of point being
    // processed and determine if this is within boundaries
    s_int32 i;
    i[0] = count(nz, 1);
    s_int32 j;
    j[0] = countChain(ny, 1, i);
    s_int32 k;
    k[0] = countChain(nx, 1, j);

    // keep track if we are inside boundaries
    s_int32 cond;
    cond[0] = (i >= a1 & i < n1) & (j >= a2 & j < n2) & (k >= a3 & k < n3);

    s_int32 func;
    func[0]=
        c * x[0] +
        c * x[1]      + c * x[-1] +
        c * x[-nx]    + c * x[nx] +
        c * x[-nx*ny] + c * x[nx*ny];


    // if inside boundaries apply the stencil operator
    // otherwise assign a default value (0)
    // int r = fselect(cond, func, 0);

    output_ic(res, func, cond);
}
