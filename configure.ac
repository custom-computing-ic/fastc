AC_INIT([FAST Compiler], [0.1], [paul.grigoras90@gmail.com],
[fastc], [https://github.com/paul-g/fastc])
AC_PREREQ([2.59])

AC_PROG_CXX

# Put configuration results here, so we can easily #include them:
# AC_CONFIG_HEADERS([config.h])

# Put autotools auxiliary files in a subdir, so they don't clutter top dir:
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.11.1 -Wall no-define foreign parallel-tests color-tests])

##########################################################################
# adding the ROSE library (static name 'librose.a')
##########################################################################

# adding the lib to the files to link
LIBS="-lrose $LIBS"

# introduce the optional configure parameter for a non-standard install prefix of ROSE
AC_ARG_WITH([rose],
[AS_HELP_STRING([--with-rose=prefix],
[try this for a non-standard install prefix of ROSE])],
[ROSEPATHSET=1],
[ROSEPATHSET=0])


# if optional parameter used, extend path flags for compliler and linker
if test $ROSEPATHSET = 1 ; then
# extend the compiler and linker flags according to the path set
AM_CXXFLAGS="$AM_CXXFLAGS -I$with_rose/include"
AM_LDFLAGS="$AM_LDFLAGS -L$with_rose/lib"
fi


##########################################################################
# adding the boost library
##########################################################################

# adding boost libs to the files to link
LIBS="-lboost_regex -lboost_filesystem -lboost_system $LIBS"

# introduce the optional configure parameter for a non-standard install prefix of ROSE
AC_ARG_WITH([boost],
[AS_HELP_STRING([--with-boost=prefix],
[try this for a non-standard install prefix of boost])],
[BOOSTPATHSET=1],
[BOOSTPATHSET=0])

# if optional parameter used, extend path flags for compliler and linker
if test $BOOSTPATHSET = 1 ; then
# extend the compiler and linker flags according to the path set
AM_CXXFLAGS="$AM_CXXFLAGS -I$with_boost/include"
AM_LDFLAGS="$AM_LDFLAGS -L$with_boost/lib"
fi

##########################################################################
# adding the libxml++ library
##########################################################################
# adding xml libs to the files to link
LIBS="-lxml++-1.0 -lxml2 $LIBS"
AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/include/libxml2"

# introduce the optional configure parameter for a non-standard install prefix of ROSE
AC_ARG_WITH([libxmlpp],
[AS_HELP_STRING([--with-libxmlpp=prefix],
[try this for a non-standard install prefix of XMLPP])],
[XMLPPPATHSET=1],
[XMLPPPATHSET=0])

# if optional parameter used, extend path flags for compliler and linker
if test $XMLPPPATHSET = 1 ; then
# extend the compiler and linker flags according to the path set
AM_CXXFLAGS="$AM_CXXFLAGS -I$with_libxmlpp/include"
AM_LDFLAGS="$AM_LDFLAGS -L$with_libxmlpp/lib"
fi

##########################################################################

AM_CXXFLAGS="$AM_CXXFLAGS -Winvalid-pch -Wall -std=c++0x -g3 -O0"

# distribute the changed variables among the Makefiles
AC_SUBST([LIBS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
