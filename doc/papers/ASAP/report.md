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

* We developed MaxC, a C based meta-programming language and API for
  specifying dataflow designs

* We implemented the backend of the MaxCC compiler, translating from
  MaxC to MaxJava [??]and other optimisations

* We evaluate our approach using various case studies including RTM, [??]

# Proposed Design Flow

We propose a design flow based on Lara and MaxCC to

with source-level transformation guided by aspects until reaching a
form with efficient implementation

We propose a design flow for generating highly efficient FPGA designs,
targeting a streaming dataflow architecture from high level languages
such as C. We use LARA to profile and optimise the original source
program, identifying potential candidates for optimisation, based on
our optimization strategies. We then compile this to two components:

1. CPU code written in C using Maxeler's runtime API
2. A DFE design, specified in an intermediate representation, MaxC, a
 dataflow language we developed as an extension of C99.

We then analyze and transform this representation with LARA and finally
compile it to MaxJava, Maxeler's own dataflow language.

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
%    \node [block, below of=decide, node distance=3cm] (stop) {stop};

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
meta-programming language in the sense that, unlike regular C
programs, a MaxC program is not intended to be executed but to specify
a hardware design. MaxC is designed to be human readable and to
facilitate integration with other tools. Being based on the C99
standard, the language interacts well with existing compilers or source
to source translation frameworks (e.g. LARA, ROSE), allowing source
level optimizations to be applied through different tools.

## Example

The example below presents a simple 1d convolution.

\fxwarning{Some background on the example...}

~~~
void kernel_Convolution1d(
  sin_float8_24 p,
  sout_float8_24 output,
  float8_24 c_0_0_0, float8_24 c_p_0_0, float8_24 c_n_0_0,
  uint32 n1, uint32 ORDER)
{
    s_float8_24 i4 = count(1000, 1);
    s_float8_24 i1 = countChain(n1, 1, i4);

    s_bool up = (i1 >= ORDER) && (i1 < n1 - ORDER);

    s_int32 result =
        p[0]  * c_0_0_0 +
        p[1]  * c_p_0_0 +
        p[-1] * c_n_0_0;

    s_int32 inter = stream_select(up, p, result);

    output_i(output, inter);
}
~~~

## Extensions


A number of extensions are supported by the maxcc compiler, without
breaking compatibility with C99. These features are meant to simplify
the language and make it more expressive as well as provide additional
verification and optimisation mechanisms (e.g. type safety). Maintaining compatibility
with C99 simplifies the kernel simulation flow as discussed below.

We provide the following extensions to C99:

* types:

    - floati_j (e.g float8_24, float10_22 etc.)
    - uinti/inti (e.g. uint32/int32)
    -


## API

## Simulation Execution Model


! Example

Features

Simulation

## Wiring

We create a simple language for specifying kernel and host interconnections.

! Example


implementation of maxcc and other optimisations

# Evaluation

evaluation using various case studies including RTM

### Himeno


## 1D Convolution ?

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
g
