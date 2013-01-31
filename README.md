MaxCC
=====

Installation
------------
To install maxcc run:
~~~~
tar xvzf maxcc-${version}.tar.gz && cd maxcc-${version}
configure --with-boost=/path/to/boost --with-rose=/path/to/rose
make && make install
~~~~

Usage
-----

~~~
maxcc /path/to/kernels.c
~~~

This will generate MaxJ design files from the specified MaxC design file (which can contain one or multiple kernels).

(NOTE! MaxCompiler Manager generation is not currently supported.)

MaxC
----

Examples of projects/kernels are listed below.

#### MaxC Designs

MaxC design files only are in:
~~~
test/maxc
    testRTMSingle.c --> Kernel for RTM
    test1dConvolutionKernel.c  --> Kernel for 1d Convolution
    testMaxCTemplate    --> Kenrel for a simple convolution
    testCmdRead.c       --> Memory Controller (Read)
    testCmdWrite.c      --> Memory Controller (Write)
~~~

These tests can be run with `make test` and cover the basic functionality of MaxC:
 1. Inputs and Outputs
    ~~~
    // Simple output of "inter"'s value to the output named "sink""
    output_i(sink, inter);

    // Conditional output, only enabled when cond is true
    output_ic(res, func, cond);

     // Output an array of floats of width Par
    output_iaf(output_pp, output_pp_inter, 8, 24, Par);

    // make a custom DRAM output stream to memory
    DRAMOutput("dram_write", Control,  burstCount + iniBursts, burst_inc, 1, 0,
               (burstCount == totalBursts - burst_inc) && iterCount == iterations - 1);
    ~~~

 2. Computation
    ~~~
    s_int32 x = ... // define a stream of int32

    // arithmetic operators are used as normal
    int32 func =
        c * x[0] +  //  extract next/past values with array index notation
        c * x[1]  + c * x[-1] +
        c * x[-2] + c * x[2] +
        c * x[-3] + c * x[3];

    ~~~

 3. Control
    ~~~
    // if up[i] == 0, select a otherwise select b as the value of result
    result = fselect(cond, a, b);

    // loops with compile time constants (only) are allowed
    for (int i=0; i <Par; i++)
        up[i] = i3>=ORDER & i3<n3-ORDER  & i2>=ORDER & i2<n2-ORDER  & i1>=ORDER-i  & i1<n1-ORDER-i;
    ~~~

The headers for these functions and others are defined in include/maxcc.h and
should be included in any MaxC design.

#### MaxC/MaxCompiler Projects

A few example MaxC/MaxCompiler projects are in:

~~~~
test/projects
    test/projects/rtm_float_multi --> Multipipe implementation for RTM
    test/project/1dConvolution    --> 1d Convolution Computation
    test/project/
~~~

These projects are strucured as follows (working example rtm_float_multi):

~~~
test/projects/rtm_float_multi
 Makefile   --> Targets for building/running the desings.
 deploy.sh  --> Can be used to deploy to remote machine and build/run the designs. (NOTE! You may have to configure this to your local setup)
 maxc/      --> [MaxC]  Contains maxc design sources. These are compiled to MaxCompiler designs in engine/
 engine/    --> [MaxJ]  Contains maxcompiler design files, generated with `make maxc`
 cpu/       --> [C/C++] Application host code with MaxCompilerRT
 dse.sh/    --> [Bash]  Usedto generate parameter headers for design space exploration
 exploded/  --> The original design is "exploded"/expanded to multiple designs with various parameter configurations
~~~
