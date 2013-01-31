MaxCC
=====

Installation instructions
-------------------------
To install maxcc run:
~~~~
tar xvzf maxcc-${version}.tar.gz && cd maxcc-${version}
configure --with-boost=/path/to/boost --with-rose=/path/to/rose
make && make install
~~~~


MaxC
----

Examples of projects/kernels are in 

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

These are tested with `make test`. They demonstrate the basic functionality of MaxC:

1. Inputs and Outputs
2. Computation
3. Control

#### MaxC/MaxCompiler Projects

MaxC/MaxCompiler projects are in:

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
