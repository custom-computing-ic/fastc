#include "../../../include/maxcc.h"

void kernel_CounterKernel() {

    // simple counter chain
    int32 count1 = count(1000, 1);
    int32 count2 = count_chain(10, 1, count1);
    int32 count22 = count_chain(20, 2, count1);

    // parametrized counters
    int32 count3  = count_p(32, 5, 1, count1);
    int32 enable = (count3 == count2);
    int32 count4 = count_p(16, 10, 2, enable);

    // 2nd simple counter chain
    int32 count31 = count(1000, 1);
    int32 count32 = count_chain(10, 1, count31);
    int32 count322 = count_chain(20, 2, count31);

}
