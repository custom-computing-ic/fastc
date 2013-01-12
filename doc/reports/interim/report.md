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
have hit a power and memory wall. Performance does not scale linearly
with technology improvement anymore (i.e. increase in number of
transistors)

Custom designs on FPGA devices can be used to improve performance and
power consumption of computationally intensive algorithms and scale
linearly with resource availability

Creating custom designs is a difficult task and requires detailed
knowledge of circuit design and underlying performance considerations

Ideally we want to synthesize custom designs from existing high level
implementations in C or other labnguages without compromising on
performance and energy efficiency.

Although there are implementations (e.g. Vivaldo, etc), non of them
targets streaming architectures [??], they combine implementation
with optimization strategies

Why aspect oriented techniques ? because they are a convenient way to
separate implementation from optimization strategy, required to meet
non functional requirements

The aim of the project is to study how techniques of Aspect Oriented
Programming can be applied to the compilation and optimisation of
designs targeting streaming Data Flow Engines. This includes
identifying efficient compiled patterns for streaming DFEs and how
such patterns can be obtained from high level descriptions using AOP
design techniques.

## Contributions

Currently we have:

* Proposed a tool chain for compiling existing programs to efficient
designs by targeting a high throughput streaming dataflow
architecture;

* Developed MaxC, a C based language, which is an intermediate step
  between C and MaxJava (Maxeler's API/language for specifying
  hardware designs )

## Planned Contributions

By the end of the project we plan to:

* We identify efficient compilation patterns and optimizations and
capture and automate them using LARA, an aspect oriented language for
embedded systems.
* We develop a compiler MaxCC, capable of automating the process
* We evaluate our solution on a number of applications, including RTM
- proposed design flow with source-level transformation guided by
  aspects until reaching a form with efficient implementation
- implementation of maxcc and other optimisations
- evaluation using various case studies including RTM

Using C is one of the limitations

How do we debug maxc designs?


- proposed design flow with source-level transformation guided by
  aspects until reaching a form with efficient implementation
- implementation of maxcc and other optimisations
- evaluation using various case studies including RTM

# Background


## "von Neumann" Architecture


* General purpose computing devices are easy to program and understand
  [??], but they are subject to the "Turing Tax": ....

* Much of a modern CPU chip's area is used by different levels of
  caching, reducing the area available for compute logic

* Multicore programing is an answer for the processor power wall
  problem but there are many [??] classes of algorithms/problems whose
  performance does not scale with the number of cores. e.g. Sparse
  Matrix Applications, Schlumberger presentations

* Custom computing devices can overcome this issue, but are
  considerably harder to program/develop

* out of order execution

* Memory wall - parallel processors/cores share same on chip memory

## Streaming Dataflow Architectures

* Diagram comparing CPU vs FPGA flow

* Domain specific data encoding

* In convolution can cascade multiple timesteps, becoming compute
  bound so we scale with transistor increase, which translates to more
  functional units

## Accelerating Program Execution



## MaxCompiler

## LARA

## Related Work

### Vivaldo

### CatapultC

### AutoEsl

### Impulse C



# Design

# Implementation

# Proposed Project Plan

# Evaluation

## 3D Convolution

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
