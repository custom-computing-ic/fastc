# Reverse Time Migration (RTM) & Ad Predictor (ADP) Demo

This demo shows:

* Use of LARA aspects to transform FAST designs:
  1. Introduce DSP balancing pragmas selectively on arithmetic statements
  2. Tune design parameters (design constants, compute types)
  3. Perform design space exploration

* Use of the FAST compiler to generate MaxJ code from
aspect-instrumented FAST programs.


## Requirements

This demo requires the LARA and Harmonic weavers to work properly.
Please download and extract the binaries (in the demo directory):

```
wget -O aspDFE.support.tgz http://goo.gl/MKmgaU && tar xvzf aspDFE.support.tgz
```

## Running

Use the provide `run` script to run the demo. This will generate
designs (as described above):

1. For RTM the source file is src/RTMKernel.c; designs are generated
in designs-RTM

2. For ADP the source file is src/ADPKernel.c; designs are generated
in designs-ADP
