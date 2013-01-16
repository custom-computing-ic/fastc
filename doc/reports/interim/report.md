# Introduction

\begin{comment}
By the time the interim report is due you should have a clearly defined
project, understand well the motivation and issues to be addressed, know
the background work in detail, have the main ideas for how to tackle the
problem and have started the development. You should also have a plan
for the remainder of the project and, importantly, how to evaluate the
project.
\end{comment}

Conventional computing devices based on general purpose processors
have hit a power and memory wall - performance no longer scales
linearly with technology improvement (i.e. increase in number of
transistors) [@survive1; @survive2]. Custom hardware designs can
improve performance and power consumption of computationally intensive
applications, allowing their performance to scale linearly with
resource availability.

However, manually creating these designs from existing high level
language implementations is a difficult task that requires detailed
knowledge of circuit design and underlying performance
considerations. So, ideally, we want to automatically generate custom
designs from existing high level implementations without compromising
on performance and energy efficiency.

The aim of the project is to study how techniques of Aspect Oriented
Programming can be applied to the compilation and optimisation of
designs targeting streaming Data Flow Engines (special computing
devices built around Field Programmable Gate Array chips that
implement the dataflow paradigm of computation). This includes
identifying efficient compiled patterns for streaming DFEs and how
such patterns can be obtained from high level descriptions using AOP
design techniques.

Our goal is to develop and implement a compilation flow from
high-level C programs to a streaming dataflow architecture that
captures and applies optimizations using LARA, an AOP language for
embedded systems [@Lara1; @Lara2].

## Contributions

* We propose an approach for compiling high level C programs to
  efficient designs targeting a high throughput streaming dataflow
  architecture;

* We introduce MaxC, a C based meta-programming language and API for
  specifying dataflow designs;

* We implemented the backend of the MaxCC compiler that translates from
  MaxC to MaxJ.

## Planned Contributions

By the end of the project we plan to:

* Evaluate our approach for compiling MaxC designs to MaxJ using a
  static and a run-time reconfigurable implementation of a high
  performance application based on the Reverse Time Migration (RTM)
  technique for seismic imaging [@fplxinyu12];

* Identify efficient compilation patterns and optimizations and
  capture and automate them using LARA and MaxCC;

* Develop the MaxCC frontend for automating the process of translating
  high level C specifications to MaxC dataflow designs;

* Evaluate our solution on a number of applications, including RTM and
  the Himeno benchmark.

# Background

In this chapter we compare streaming data flow architectures with
traditional, general purpose architectures and we describe the Maxeler
hardware acceleration solution and the MaxCompiler toolchain and API
which represent the target of the MaxC compilation process. We also
look at the LARA language which will be used as part of the design
flow to specify and apply optimzation strategies both to the original
source code and to the resulting dataflow desing. We also summarize
related work in the area of high level synthesis tools.

## Streaming Dataflow Architectures

Although general purpose computing devices offer a convenient
programming paradigm, the traditional fetch - decode - execute cycle
is inherently sequential and relies on inefficient access to external
memory. To compensate for this a large area of a modern CPU core is
dedicated to caches, branch prediction units and out-of-order
scheduling and retirement units. This reduces the area of the chip
available for performing useful computation. Furthermore, although
multicore programing is an answer for the processor power wall (which
prevents increases in operating frequency beyond a certain point,
limiting the processing speed of a single core device), there are
classes of algorithms whose performance does not scale linearly with
the number of cores. This is especially true when operating on large
volumes of data with poor spatial locality that do not fit into a
CPU's on-chip cache such as algorithms involving sparse matrix
computations or convolution [@survive1]. Although this model offers
good flexibility when dealing with arbitrary access patterns, it is
not efficient for large volumes of highly regular data.

The dataflow computing paradigm operates differently form the general
purpose computing paradigm (as shown in Figure \ref{fig:cpudfe}),
being designed to be efficient at processing large volumes of data. It
works by creating a streaming dataflow graph of computational nodes,
which operates as a large computational pipeline: input data is
streamed in sequentially through each pipeline stage and output data
is streamed out. This results in a highly pipelined design that can be
statically scheduled achieving throughput rates of one value per cycle
by completely avoiding pipeline hazards. This means that a design
running at a few hundred megahertz can easily outperform a CPU
implementation running at a few gigahertz while being more energy
efficient [@survive2].


\begin{figure}[h] \centering
\includegraphics[scale=0.45, trim=0 200 0 150]{res/cpu-vs-dfe.png}
\caption{Comparison between general purpose CPU architecture and a
streaming Data Flow Engine. In the case of the latter instructions are
not stored in memory but encoded in the dataflow graph. }
\label{fig:cpudfe}
\end{figure}

## Maxeler Acceleration Solution

Maxeler Technologies provides a software and hardware acceleration
solution based on the dataflow computing model. The dataflow design is
created using MaxCompiler [@maxwhite] and implemented on a specialized
hardware platform, built around high-end Field Programmable Gate Array
(FPGA) chips.

FPGAs are logic chips that can be reconfigured in seconds to implement
custom applications. Hence they offer a much shorter time to market
than traditional ASIC[^asic] based solutions, while still being able
to implement custom logic circuits, making them significantly faster
than general purpose hardware. However, the size of the FPGA chip
constrains the design that can be uploaded onto the chip. FPGAs have a
limited number of each of the following resource types:

* lookup tables (LUTs) - implement the logical functions performed by the circuit;

* flip-flops (FFs) - small storage elements;

* digital signal processors (DSP) - small special purpose arithmetic units;

* block RAM (BRAM) - larger, on-chip storage elements.

The specific data flow engine used for this project is a MAX3424A card
based on a Virtex 6 FPGA chip [@virtex6]. The MAX3 provides 48GB of
on-board DRAM and about 4MB of fast on-chip BRAM are available on the
FPGA chip.

The system is connected to the dataflow engine via PCIe as shown in
Figure \ref{fig:max3}.

\begin{figure}[h]
\centering
\includegraphics[scale=0.45, trim=0 200 0 200]{res/max3.png} \caption{
The Maxeler acceleration solution: the DFE is connected to
the host machine via PCIe. The board comprises 48GB of DRAM and a
Virtex 6 FPGA chip. }
\label{fig:max3}
\end{figure}

[^asic]: Application Specific Integrated Circuit

## MaxCompiler

MaxCompiler is a high level compiler targeting the acceleration
platform developed by Maxeler Technologies [@maxwhite]. It provides a
Java based API for specifying hardware designs that are compiled and
uploaded onto the DFE and a C runtime interface for the part of the
application running on the CPU of the host system.

We demonstrate the use of MaxCompiler in accelerating a simple moving
average computation, starting from an original design in C shown in
Listing \ref{MovingAvg-C}. This performs a three point moving average
computation on an input array x, using 2 point averages at boundaries.

\lstset{language=C++, caption={Original three point moving average
computation in C.}, label={MovingAvg-C}}

~~~
for (int i = 0; i < n; i++ ) {
   sum = x[i], divisor = 1;
   if ( i > 0 )
     sum += x[i - 1], divisor++;
   if (i < n - 1)
     sum += x[i + 1], divisor++;
   y[i] = sum / divisor;
}
~~~

Transforming this code to use the Maxeler acceleration platform
requires us to write three programs.

First we must create a kernel design. This is written in Java and
specifies the computational datapath. Listing \ref{MovingAvg-Kernel}
shows some of the important features of the MaxCompiler API:

- kernel inputs and outputs provide an I/O interface that allow the
  kernel to communicate with the rest of the design (Lines 4 and 15);

- stream offset expressions allow accessing past and future elements
  of a stream (Lines 5 and 6). The offset window is stored into on
  chip BRAM so is limited to a few tens of thousands elements;

- frequently used components such as counters are provided by the
  API. they are useful in keeping track of iteration count when
  mapping loops to streaming designs (Line 7);

- operator overloading is used to perform arithmetic on input streams;

- multiplexers (in this instance represented by the overloaded
  conditional operator) are used to select between streams (Lines 11
  and 12).

\lstset{language=Java, label={MovingAvg-Kernel}, caption={Kernel
design for the three point moving average computation.}}

~~~
public class MAKernel extends Kernel{
  public MAKernel(KernelParameters parameters) {
    super(parameters);
    HWVar x = io.input("x", flt );
    HWVar x prev = stream.offset(x, -1);
    HWVar x next = stream.offset(x, +1);
    HWVar cnt = control.count.simpleCounter(32, N);
    HWVar sel nl = cnt > 0;
    HWVar sel nh = cnt < (N - 1);
    HWVar sel m = sel_nl & sel nh;
    HWVar prev = sel_nl ? x prev : 0;
    HWVar next = sel_nh ? x next : 0;
    HWVar divisor = sel_m ? 3.0 : 2.0;
    HWVar y = (prev+x+next)/divisor;
    io.output("y" , y, hwFloat(8, 24) );
  }
}
~~~

Next we create a manager design, also written in Java; this is used to
manage kernel I/O, connecting multiple kernels together (in multi
kernel designs) and kernels to DRAM and the CPU interface (via
PCI-E). Listing \ref{MovingAvg-Manager} shows a simple design for the
moving average application that instantiates a single moving average
kernel and connects its inputs and outputs to the host interface.

\lstset{label={MovingAvg-Manager}, caption={Manager design for the
three point moving average computation}}

~~~
public class MAManager extends CustomManager {
  public MAManager(MAXBoardModel board_model, boolean is_simulation, String name) {
    super(board_model, is_simulation, name);

    KernelParameters params = manager.makeKernelParameters("MAKernel");
    KernelBlock k = addKernel(new MovingAverageKernel(params));

    k.getInput("x") <== addStreamFromHost("x");
    addStreamToHost("y") <== k.getOutput("y");

  }
}
~~~

Finally we must write a host application which is required to queue
the input streams and run the accelerator. This is achieved by calls
to the MaxCompilerRT (runtime) API that interfaces with MaxelerOS.

\lstset{label={MovingAverage-Host}, caption={Host example for queueing
the input and output streams and running the three point moving
average design.}}

~~~
max_run(
  device,
  max_input("x", x, x_size),
  max_output("y", y, y_size),
  max_runfor("MAV", n),
  max_end());
~~~


Although the MaxCompiler toolchain greatly simplifies the process of
accelerating applications and particularly the designing of dataflow
kernels, the acceleration process is still very involved and requires
a large amount of experience with FPGA technology and domain specific
knowledge. Most importantly the whole process is manually driven
including the exploration of optimizations. This step is a critical
and time consuming part of the design process which is vital in
achieving maximum performance (in terms of operating frequency, number
of parallel pipelines etc.) subject to physical limitations such as
chip size or timing constraints.

By specifying the design in MaxC and optimization strategies in LARA
we aim to create the basis of a design space exploration flow that can
automate this process.

## LARA

Lara is an aspect oriented programming language for specifying
compiler strategies for FPGA-based systems
[@Lara1; @Lara2; @Lara3]. The key feature of LARA is that it separates
the application code from the strategies required to compile and
optimize it for a particular platform. It enables the capturing of
strategies for:

* instrumentation, monitoring and hardware-software partitioning
  [@Lara2]- these will be used in the initial stages of our proposed
  design flow to identify optimization candidates for mapping onto
  the accelerator

* code specialization and code optimization [@Lara2] -- these will be
  used both for the original source code application which is the
  target of the compilation and for the dataflow design

Furthermore LARA descriptions can be parametrized which enables us to
use it in our proposed DSE[^DSE] step.

[^DSE]: Design Space Exploration

\begin{comment}

## ROSE Compiler Infrastructure


## Related Work

### Vivado

### CatapultC

### AutoEsl

### Impulse C

\end{comment}

# Proposed Project Plan

Table 3.1 shows the proposed project
milestones along with their expected completion date and current
status. We have currently completed the first three of the nine
proposed milestones and are working towards the fourth one.

The first milestone involved background reading on the tools used for
this project particularly MaxCompiler and LARA. We also analysed a
number of compiler frameworks to use for implementing MaxCC including
LLVM[@llvm], Cetus[@cetus] and ROSE[@rose]. We developed a small
code-generation prototype using Cetus but did not find the exisiting
infrastructure satisfying or flexible enough. We opted for ROSE which
is a mature framework with integrated support for C99/C++ AST
generation. The milestone also involved acquiring more in-depth
knowledge of the C++ language and GNU toolchain (e.g Autotools[@autotools]).

The second milestone involved designing the MaxC language. This meant
analysing exisiting MaxJ applications as well as the MaxJ API and
identifying required functions such as suport for stream offsets,
multiplexing etc. We used the RTM application as a case study to guide
our design of the language.  The MaxC language is briefly described in
Section \ref{maxc}.

For the third milestone we implemented the MaxCC backend, responsible
for translating the MaxC design to MaxJ. This is briefly described in
Section \ref{maxcc-impl}.

During milestones 4 and 5 we will evaluate our approach for compiling
MaxC designs using the RTM case study and identify potential design
optimizations which we will capture using LARA.

For milestones 6, 7 and 8 we are concerned with identifying and
implementing a compilation strategy from C source to MaxC guided by
LARA aspect-based optimization strategies.

The final milestone is allocated for evaluating the entire project
(our approach is described in Section \ref{sec:evaluation}) and
writing the final report.


 **No**  **Milestone**                                    **Date**   **Status**
-------- -------------                                   ---------- ------------
1.       Background reading (MaxCompiler, LARA, ROSE)    Nov 2012     Completed
2.       Design the MaxC language                        Dec 2012     Completed
3.       Implement the MaxCC backend                     Dec 2012     Completed
4.       Implement RTM using MaxC                        Jan 2013     In progress
5.       Identify LARA strategies for MaxC designs       Jan 2013     Planned
6.       Design and implement the MaxCC frontend         Feb 2013     Planned
7.       Implement RTM in C                              Mar 2013     Planned
8.       Identify LARA strategies for C applications     Mar 2013     Planned
9.       Evaluation and Report Writing                   Jun 2013     Planned

Table: Project milestones along with expected completion dates and
current status.

# Proposed Design Flow

We propose a design flow for generating efficient FPGA designs,
targeting a streaming dataflow architecture from high level languages
such as C. We target the MAX3424A card from Maxeler Technologies,
using MaxCompiler 2012.1 to drive the backend compilation process. The
flow consists of the following steps:

1. We use LARA to profile the original source program, identifying
   potential candidates for optimisation;

2. We use LARA to perform hardware/software partitioning
   [@hwswcodesign] transforming the source program by extracting
   acceleration candidates in separate kernel functions and applying
   optimisation strategies from the repository;

3. We generate a runtime component to be executed on the host
   processor that will configure the accelerator design, queue the
   data to be processed and process the results; this is written in C
   using Maxeler's runtime API;

4. Using the MaxCC frontend we generate a dataflow design from the
   processed C source, comprising one or more specialized kernels
   specified using MaxC, a dataflow language we developed as an
   extension of C99.

5. We perform DSE until we find a design that fits the chip, achieves
   timing closure and meets non functional requirements.  For each
   optimization strategy in our repository we:
       - apply the set of specific low-level optimizations comprised
         in the strategy (e.g. setting DSP balance).
       - we compile the optimized design using the MaxCC backend to
         MaxJ, Maxeler's own dataflow language
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

## MaxC

\label{maxc}

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

\lstset{style=MaxC, label={conv1d-maxc}, caption={Example 1D
Convolution kernel implemented in MaxC.}}

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

## Implementation

\label{maxcc-impl}

The MaxCC backend is implemented using the ROSE compiler framework
[@rose00]. It processes the Abstract Syntax Tree generated by the ROSE
frontend into a data flow graph representation of the original MaxC
design. It then traverses this graph in conjunction with the original
AST emitting MaxJ code.

MaxCC supports the following types, as extensions to C99:

- `floati_j` (e.g `float8_24`, `float10_22` etc.), `uinti/inti`
  (e.g. `uint32`, `int32`) allow specification of custom width data
  types

- `s_floati_j`, `s_inti_j` etc. (e.g. `s_float8_24`) allow specification of
  stream data types

To allow decoupling of bit width specifications from the functional
code we also provide the type `s_float`, `s_int` etc. In this
situation we can specify the type using a pragma directive.

The previous example also illustrates some of the MaxC API components:

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

\lstset{style=MaxC, label={simloop}, caption={Kernel simulation loop.}}

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

\lstset{style=MaxC, label={simexample}, caption={Simple example for
simulation code.}}

~~~
void kernel_Convolution1d(
    sin_float8_24 in,
    sout_float8_24 out,
    float8_24 c)
{
    float8_24 count = counter(10, 1, 0);

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

\lstset{style=MaxCconf, label={maxconf}, caption={Example
configuration file for the RTM application}}

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

\label{sec:evaluation}

The purpose of our evaluation strategy is twofold:

1. first we should confirm that our compilation startegy succesfully
handles a meaningful set of C applications;

2. second we should evaluate the performance of the compiled designed
by comparing to existing high-performance applications.

For this purpose we propose to evaluate the MaxC compiler using two
high performance applications, which have published results for
implementations targeting the Maxeler dataflow platform. This enables
us to evaluate the quality of the compilation process itself, rather
than the underlying hardware platform.

## RTM

We propose to evaluate our project using the MaxC compiler to generate
three different implementations of a high performance application
based on the Reverse Time Migration (RTM) technique for seismic
imaging [@fplxinyu12]:

* first, we will implement the simplest version, a design with a
  single computational pipeline (or core) on the FPGA;

* the second design is a multi pipe version, achieving more efficient
  area and memory bandwidth utilization;

* finally we aim to produce a run-time reconfigurable version as
  described in [@fplxinyu12].

In all three cases we will analyze both area and performance results
of the optimization strategies applied using MaxC and LARA and compare
them with the reference implementation described in
[@fplxinyu12]. This is currently the fastest published result for an
RTM implementation for FPGA or GPU results and up to two orders of
magnitude faster than CPU implementations. Hence, being able to
generate, in a fully automated fashion, an efficient design with
similar performance would represent a significant achievement.

## Himeno

If time permits we would also like to evaluate MaxCC by implementating
the Himeno benchmark [@himeno], a memory bound benchmark used by the
HPC community. We would compare our results with those published in
[@max-himeno], a manual implementation which also targets the Maxeler
dataflow acceleration solution.

# References
