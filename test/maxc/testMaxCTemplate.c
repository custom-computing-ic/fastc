typedef int* s_int32;
typedef int int32;

extern int count(int a, int b);
extern int count_chain(int a, int b, int parent);
extern void output_ic(int* res, int func, int32 cond);

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
void kernel_KernelFunc(s_int32 x, int *res,
                       int nx, int ny, int nz,
                       int a1, int a2, int a3,
                       int n1, int n2, int n3,
                       int c) {

    // counters are required to keep track of point being
    // processed and determine if this is within boundaries
    int32 i = count(nz, 1);
    int32 j = count_chain(ny, 1, i);
    int32 k = count_chain(nx, 1, j);

    // keep track if we are inside boundaries
    int32 cond = (i >= a1 & i < n1) & (j >= a2 & j < n2) & (k >= a3 & k < n3);

    int32 func =
        c * x[0] +
        c * x[1]  + c * x[-1] +
        c * x[-2] + c * x[2] +
        c * x[-3] + c * x[3];


    // int r = fselect(cond, func, 0);
    output_ic(res, func, cond);
}
