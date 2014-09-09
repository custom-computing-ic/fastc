fastc
=====

Experimental framework for aspect-oriented compilation of dataflow designs.

This is described in [Paul Grigoras et al., _Aspect Driven Compilation for Dataflow Designs_](http://paul-g.github.io/pgasap2013.pdf)


Cloning This Repo
-----------------

`git clone https://paulg90@bitbucket.org/paulg90/fastcc.git`

`git submodule init && git submodule update`

Installation
------------

~~~~
tar xvzf fastc-${version}.tar.gz && cd fastc-${version}

./autogen.sh --with-boost=/path/to/boost --with-rose=/path/to/rose

make && make install
~~~~

Usage
-----

~~~
fastc I/path/to/fastc/include/ /path/to/kernels.c
~~~

For example:

~~~
fastc -Iinclude/ test/testRTMStatic/maxc/RTM.c
~~~

This will generate MaxJ files from the specified FAST file.

(NOTE! MaxCompiler Manager generation is not currently supported.)
