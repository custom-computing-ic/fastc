TODOS
-----
Find a simple example to showcase the flow of optimising on CPU vs FPGA.
Ideally should be able to use this during the demo.

Possibilities
1. Moving average
2. Sum



Narrative
=========

1. CPU implementations are easy to write, optimisation space is
smaller. But generality causes it to be less efficient (power +
performance)

2. Can fix this with _custom_ dataflow designs, by creating
application specific circuits. But the high level of flexibility leads
to a large design space -- hard to explore manually.
Table showing some dataflow speedup results

3. This project is about:
* improving productivity
* maintaining/improving efficiency

4. How can we achieve this?
Show the diagram of a dataflow design/ C99 specification
Show an example of dataflow/C99 code showing mixing optimisations and application code

4.1 Key idea is to decouple optimisations from design development:
* this leads to cleaner design specification
* more maintainable code
* portability of optimisations
* improved performance via automated design space exploration

5. How to decouple? Use aspect-oriented programming
* introduce aspect-oriented programming: motivation, encapsulate cross-cutting concerns
* however, this is usually done at run-time; we need something that
  works at compile-time, finer pointcut model
* solution: use LARA -> developed as part of the HARNESS project, etc.

6. Plan:
* encapsulate optimisations in LARA
* apply optimisations to dataflow designs to explore design space
* generate multiple implementations, executed and measure quality metrics
* choose a best implementation

7. Problems:
* LARA only supports C99 compliant syntax
* Our back-end is MaxCompiler (only option), Java based
* Solution: introduce a simple language on top of MaxCompiler
* Simplest possible is C
* But, can we capture all features of MaxCompiler in C?

8. Introduce FAST(Facile Aspect-driven Source Transformation)
* C99 style syntax
* dataflow execution model
* capture only the simplest form of dataflow specification
* use language constructs as much as possible (no API/pragmas)
* use pragmas for optimisations:
   * simple fallback options
   * problem: mixing optimisations with application code
   * solution: apply pragmas automatically using aspect

9. Other interesting FAST features:
[Key idea here -> show how everything is compiled HARDWARE, this is interesting, not the syntax]
* support hardware/software co-design: both specifications in same file/language/project
* better communication/parameter sharing between models
* support streams
* support arithmetic
* support compile-time loops
* support optimisations

11. Revised flow
Use diagram form report.

Exemplify design flow:
* Simple FAST example showing all the features
* An abstract aspect description
* Generate code
* Generate MaxCompiler design

Aspect Descriptions

Use material from ASAP

Evaluation

Use material from ASAP

Use material form Report evaluation, especially LOC

Conclusion
* Introduced a novel design flow
* Shown how this can be used to increase productivity, with minimal performance overhead

Outcome
* Full paper published in ASAP
* Project included in HARNESS project -- FP7 funded project for
  investigating heterogeneous cloud systems
*


Demo

Questions

Extra material?
