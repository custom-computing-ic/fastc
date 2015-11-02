fastc
=====

Experimental framework for aspect-oriented compilation of dataflow designs.

This is described in [Paul Grigoras et al., _Aspect Driven Compilation for Dataflow Designs_](http://www.doc.ic.ac.uk/~pg1709/pgasap2013.pdf)


Cloning This Repo
-----------------

```
git clone https://github.com/custom-computing-ic/fastc.git
cd fastc && ./autogen.sh --with-boost=/path/to/boost --with-rose=/path/to/rose
make
```

Run tests with `make check:

```
bash-4.1$ make check
make  check-TESTS
make[1]: Entering directory `/homes/pg1709/workspaces/fastc'
make[2]: Entering directory `/homes/pg1709/workspaces/fastc'
PASS: test/common.sh
PASS: test/highlevel.sh
PASS: test/applications.sh
FAIL: test/new.sh
==========================================
1 of 4 tests failed
See ./test-suite.log
Please report to paul.grigoras90@gmail.com
==========================================
```

__Note__ Tests in `new.sh` are expected to fail, they are currently unsupported features. 

Usage
-----

~~~
fastc -I/path/to/fastc/include/ /path/to/kernels.c
~~~

Example:

~~~
fastc -Iinclude/ test/testRTMStatic/maxc/RTM.c
~~~

This will generate MaxJ files from the specified FAST file.

(NOTE! MaxCompiler Manager generation is not currently supported.)
