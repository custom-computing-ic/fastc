% MaxC
% Paul Grigoras
% January 2013

# Introduction

 Custom designs on FPGA devices can be used to improve performance and
power consumption of computationally intensive algorithms. However,
creating custom designs is a difficult task and requires detailed
knowledge of circuit design and underlying performance
considerations. Ideally we want to synthesize custom designs from
existing high level implementations in C or other languages without
compromising on performance and energy efficiency.

We propose a compilation flow from high-level C programs to a
streaming dataflow architecture. Our approach captures and applies
optimizations using LARA, an aspect oriented programming language for
embedded systems. [??]

## Contributions

* We propose a tool chain for compiling existing programs to efficient
  designs by targeting a high throughput streaming dataflow
  architecture;

* We introduce MaxC, a C based meta-programming language and API for
  specifying dataflow designs

* We implemented the backend of the MaxCC compiler, translating from
  MaxC to MaxJava [??]and other optimisations

* We evaluate our approach using various case studies including RTM, [??]

# Proposed Design Flow

We propose a design flow for generating efficient FPGA designs,
targeting a streaming dataflow architecture from high level languages
such as C. The flow consists of the following steps:

1. We use LARA to profile the original source program,
   identifying potential candidates for optimisation;

2. We use LARA to transform the source program extracting acceleration
   candidates in separate kernel functions and applying optimisation
   strategies from the repository;

3. We generate a runtime component to be executed on the host
   processor that will configure the accelerator design, queue the
   data to be processed and process the results; this is written in C
   using Maxeler's runtime API;

4. Using the MaxCC frontend we generate a dataflow design from the
   processed C source, comprising one or more specialized kernels
   specified using MaxC, a dataflow language we developed as an
   extension of C99.

5. We perform a DSE step until we find a design that fits the chip,
   achieves timing closure and meets non functional requirements.
   For each  optimization strategy in our repository we:
       - apply the set of specific low-level optimizations comprised
         in the strategy (e.g. setting DSP balance).
       - we compile the optimized design using the MaxCC backend to
         MaxJava, Maxeler's own dataflow language
       - we start the backend compilation toolchain (MaxCompiler and
          Xilinx) and perform an analysis of the reporting
          information, based on which we either restart the flow with
          the next optimization strategy step or proceed to the next
          step
       - we measure NFRs such as performance or latency and if our
         target is not met we restart using a different optimization
         strategy

\begin{center}
\begin{tikzpicture}[node distance = 2cm, auto]
    \node [block] (csrc) {C source};
    \node [block, below of=csrc, left of=csrc] (maxrt) {CPU Runtime code};
    \node [block, below of=csrc, right of=csrc] (maxc) {MaxC Design};
    \node [block, below of=maxc] (maxj) {MaxJ Design};
    \node [block, below of=maxj] (maxfile) {Maxfile};
    \node [block, below of=maxfile, left of=maxfile] (app) {Application executable};
    \node [block, below of=app] (maxnode) {MaxNode, MaxStation};

    \node [cloud, right of=csrc] (lara) {LARA};
    \node [cloud, left of=csrc] (maxcc) {MaxCC};
%   \node [cloud, right of=init] (system) {system};
%    \node [block, below of=init] (identify) {identify candidate models};
%    \node [block, below of=identify] (evaluate) {evaluate candidate models};
%    \node [block, left of=evaluate, node distance=3cm] (update) {update model};
%    \node [decision, below of=evaluate] (decide) {is best candidate better?};
%    \node [belock, below of=decide, node distance=3cm] (stop) {stop};

    \path [line] (csrc) -- (maxrt);
    \path [line] (csrc) -- (maxc);
    \path [line] (maxc) -- (maxj);
    \path [line] (maxj) -- (maxfile);
    \path [line] (maxfile) -- (app);
    \path [line] (maxrt) -- (app);
    \path [line] (app) -- (maxnode);
    \path [line, dashed] (lara) -- (csrc);
    \path [line, dashed] (lara) -- (maxc);

%    \path [line] (decide) -| node [near start] {yes} (update);
%    \path [line] (update) |- (identify);
%    \path [line] (decide) -- node {no}(stop);
%    \path [line,dashed] (expert) -- (init);
%    \path [line,dashed] (system) -- (init);
%    \path [line,dashed] (system) |- (evaluate);
\end{tikzpicture}
\end{center}

# MaxC & the MaxCC backend

MaxC is a programing language and API based on the C99 standard. It
provides a succinct means of specifying dataflow designs and is an
intermediate representation in the proposed design flow. MaxC is a
meta-programming language in the sense that, unlike a regular C
program, a MaxC program is not "executed"" but specifies
a hardware design. MaxC is designed to be human readable and to
facilitate integration with other tools. Being based on the C99
standard, the language interacts well with existing compilers or source
to source translation frameworks (e.g. LARA, ROSE), allowing source
level optimizations to be applied through different tools.

The example below presents a simple kernel performing a 1d
convolution.

Lines 2-6 show the kernel declaration specifying the stream input "p"
and output "out" as well as a number of scalar parameters configurable
at run-time.

On lines 13-16 we use array index notation to access future (positive
offset) or past (negative offset) stream elements.

Normal arithmetic operations can be used to operate on stream
elements.

\lstset{style=MaxC}

~~~
#pragma stream:result type:uint32 func:kernel_Convolution1d
void kernel_Convolution1d(
  sin_float8_24 p,
  sout_float8_24 out,
  float8_24 c_0_0_0, float8_24 c_p_0_0, float8_24 c_n_0_0,
  uint32 n1, uint32 ORDER)
{
    s_float8_24 i4 = count(1000, 1);
    s_float8_24 i1 = countChain(n1, 1, i4);

    s_int result =
        p[0]  * c_0_0_0 +
        p[1]  * c_p_0_0 +
        p[-1] * c_n_0_0;

    s_bool up = (i1 >= ORDER) && (i1 < n1 - ORDER);
    s_int32 inter = stream_select(up, p, result);

    output_i(out, inter);
}
~~~

The example illustrates the additional types we introduce as
extensions to C99. These are meant to simplify the language and make
it more expressive as well as provide additional verification and
optimisation mechanisms (e.g. type safety). Maintaining compatibility
with C99 simplifies the kernel simulation flow as discussed below.

MaxCC supports the following types, as extensions to C99:

- `floati_j` (e.g `float8_24`, `float10_22` etc.), `uinti/inti`
  (e.g. `uint32`, `int32`) allow specification of custom width data
  types

- `s_floati_j`, `s_inti_j` etc. (e.g. `s_float8_24`) allow specification of
  stream data types

To allow decoupling of bit width specifications from the functional
code we also provide the type `s_float`, `s_int` etc. In this
situation we can specify the type using a pragma directive.

The example also illustrates some of the MaxC API components:

 * output functions (e.g. `output_i`) are used to connect an internal
   kernel stream to the output stream of a kernel

 * various counters and counter chain configurations can be
   instantiated using functions from the counter API as shown on lines
   8-9

 * functions such as `stream_select` are used to multiplex between a
   number of streams based on the value of condition stream

## Kernel Simulation

The goal for the kernel simulation model is that it should be possible
to compile and run it using the standard GCC toolchain, verifying the
logical correctness of the design (i.e. not accounting for hardware
effects such as stalls etc.).

To achieve this we provide the type extensions described above as type
definitions. Streams are modelled as pointers: `typedef float*
s_float8_24`. After each kernel cycle, stream pointers are
incremented, as shown in the simulation loop below:

\lstset{style=MaxC}

~~~
for (int i = 0 ; i < CYCLE_COUNT; i++) {
   kernel_Convolution1d(p, out, 2);
   update_stream_pointers(p, out);
}
~~~

The major limitation of this approach is that, due to the lack of
support for polymorphism in C, in order to enable simulation support,
users are required to add boilerplate code, that would otherwise not
be required by the MaxCC backend. When compiling with MaxCC this
boilerplate code is simply ignored, so any mistakes in this code will
not be captured by the simulation model.

This issue is illustrated in the example below where we must use the
`stream_init_i(int stream_id)` function, passing in a unique stream id
to retrieve the appropriate stream values from the global data
structures they are stored in. The function then either allocates a
new stream or increments the stream pointer if the stream has already
been allocated. Furthermore if we extend the example to use two
counters, we similarly require unique identifiers for counters.

~~~
void kernel_Convolution1d(
    sin_float8_24 in,
    sout_float8_24 out,
    float8_24 c)
{
    float8_24 count = counter(10, 1);

    s_int32 result = stream_init_i(0);
	result[0] = c * (in[0] + in[1]);

    int32 good = (count > 2) && (count < 5);

    int32 inter = stream_selectfi(good, in, result);

    output_i(out, inter);
}

~~~



## Connecting Kernels

We create a simple configuration language for specifying kernel and
host interconnections. The configuration file used for the RTM
application is shown as an example below. This allows us to specify
the kernel instances that form our design. The configuration file has
two parts. First we specify the kernel instances. For example line 3
in the listing below specifies that we will be creating 3 kernel
instances based on a kernel named Cmdread. After specifying all kernel
instances, we can specify the flow of data between kernels, host and
memory. We can specify three types of flow:

* kernel to kernel for example `Myapp0[out] > Myapp1[in]`. This will
  connect the output of Myapp0 named "out" to the input of Myapp1
  named "in";

* memory to kernel, as shown on line 10 in the listing below; this
  will connect a dram stream named "dram2knl0" to the input of Myapp0
  "in"; the read sequence is provided by the output of Cmdread0 named
  dram_read; we can also specify a kernel to memory direction, as
  shown on line 15 in which case the second argument specifies the
  stream providing the write sequence;

* finally, we can create host to memory and memory to host connections
  as shown on lines 7 - 8.

\lstset{style=MaxCconf}

~~~
kernels:
  Cmdread:  Cmdread0 Cmdread1 Cmdread2 Cmdread3 Cmdhostread
  Cmdwrite: Cmdwrite0 Cmdwrite1 Cmdhostwrite
  RTM:      MyApp0

flow:
  Host > Memory[dram2mgr, Cmdhostwrite.dram_write]
  Memory[mgr2dram, Cmdhostread.dram_read] > Host

  Memory[dram2knl0, Cmdread0.dram_read] > Myapp0[burst_p]
  Memory[dram2knl1, Cmdread1.dram_read] > Myapp0[burst_pp]
  Memory[dram2knl2, Cmdread2.dram_read] > Myapp0[burst_dvv]
  Memory[dram2knl3, Cmdread3.dram_read] > Myapp0[burst_source]

  MyApp0[ker_p] > Memory[knl2dram0, Cmdwrite0.dram_write]
  MyApp0[output_pp] > Memory[knl2dram1, Cmdwrite1.dram_write]
~~~



# Evaluation

## RTM

Scan ocean floor to find oil and gas [Surface? HUGE]
Huge data volume, complex physics [@flynn]

Isotropic vs anisotropic

* Isotropic
* TTI
* VTI
* 30 Hz vs 60 Hz

* Convolution
* Sparse Matrix

# References
