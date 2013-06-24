fastc
=====

Cloning This Repo
-----------------

This project uses the external project [refdb](https://github.com/paul-g/refdb) 
as a submodule, so you must remember to also run the submodule init & update functions after
cloning the repository, if you intend to build documentation  that depends on this reference database.

`git clone https://github.com/paul-g/maxcc.git`

`git submodule init && git submodule update`

Installation
------------
To install fastc run:
~~~~
tar xvzf maxcc-${version}.tar.gz && cd maxcc-${version}
configure --with-boost=/path/to/boost --with-rose=/path/to/rose
make && make install
~~~~

Usage
-----

~~~
maxcc /path/to/kernels.c
~~~

This will generate MaxJ design files from the specified FAST design file (which can contain one or multiple kernels).

(NOTE! MaxCompiler Manager generation is not currently supported.)

Please check the [wiki](https://github.com/paul-g/maxcc/wiki) to get started!
