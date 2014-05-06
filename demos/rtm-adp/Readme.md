# Reverse Time Migration (RTM) & Ad Predictor (ADP) Demo

This demo shows:

* Use of LARA aspects to transform FAST designs:
  1. Introduce DSP balancing pragmas selectively on arithmetic statements
  2. Tune design parameters (design constants, compute types)
  3. Perform design space exploration
  4. Re-use of aspects for multiple designs (same aspects for RTM and ADP)

* Use of the FAST compiler to generate MaxJ code from
aspect-instrumented FAST programs.

## Running

Use the provide `demo.sh` script:

1. to setup the demo with `bash demo.sh -s`

2. to run the script with `bash demo.sh -r`

3. to package the demo for distribution `bash demo.sh -p`

After setting up the demo you should see the following structure:

```
root/
    bin/       # Binaries required for the demo
	include/   # Includes required for the demo
    demo/
        src/   # Kernel source files
		maxcc-includes/
		aspects/
        demo.sh
```

__Note__ You can change `settings.sh` to set a different path to the
`bin/` and `aspects/` directories. (Note that the `include` directory
should be in the same parent directory as the `bin/` folder).

## About

The demo operates on the RTM and ADP kernels in the `src` directory,
producing new FAST designs (as described above). The FAST designs are
then compiled to MaxJ using the FAST compiler.
