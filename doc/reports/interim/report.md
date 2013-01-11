# Introduction

\begin{comment}
By the time the interim report is due you should have a clearly defined
project, understand well the motivation and issues to be addressed, know
the background work in detail, have the main ideas for how to tackle the
problem and have started the development. You should also have a plan
for the remainder of the project and, importantly, how to evaluate the
project.
\end{comment}

The aim of the project is to study how techniques of Aspect Oriented
Programming can be applied to the compilation and optimisation of
designs targeting streaming Data Flow Engines.

The project would include identifying efficient compiled patterns for
streaming DFEs and how such patterns can be obtained from high level
descriptions using AOP design techniques.

* Custom designs on FPGA devices can be used to improve performance
  and power consumption of computationally intensive algorithms
* Creating custom designs is a difficult task and requires detailed
  knowledge of circuit design and underlying performance
  considerations
* Ideally we want to synthesize custom designs from existing high
  level implementations in C or other labnguages without compromising
  on performance and energy efficiency.
* We propose a tool chain for compiling existing programs to efficient
designs by targeting a high throughput streaming dataflow
architecture;
* We identify efficient compilation patterns and optimizations and
capture them using LARA, an aspect oriented language for embedded systems.
* We develop a language based on C, MaxC, which is an intermediate
  step between C and MaxJ (Maxeler's API for providing )
* We develop a compiler MaxCC, capable of automating the process
* We evaluate our solution on a number of applications, including RTM

- proposed design flow with source-level transformation guided by
  aspects until reaching a form with efficient implementation
- implementation of maxcc and other optimisations
- evaluation using various case studies including RTM

# Background

## Accelerating Program Execution

## Streaming Dataflow Architectures

## MaxCompiler

## LARA

# Related Work

## Vivaldo

## CatapultC

## AutoEsl

# Design

# Implementation

# Proposed Project Plan

# Evaluation

% Local Variables:
% eval: (add-hook 'after-save-hook 'recompile)
% End:
