# this is example-file: configure.ac

# initial information about the project
AC_INIT([mylib],[2.0],[me@myLib.org])

# check if the source folder is correct
# AC_CONFIG_SRCDIR([proy/src/hello/helloWorld.cpp])

# Checks for programs

# check for C++ compiler and the library compiler
AC_PROG_CXX
AC_PROG_RANLIB

# automake initialisation (mandatory) and check for minimal automake API version 1.9
AM_INIT_AUTOMAKE([foreign 1.9 no-dependencies subdir-objects])
AM_MAINTAINER_MODE

AM_PROG_CC_C_O

# ACLOCAL_AMFLAGS = -I m4
# AC_CONFIG_AUX_DIR([tmp-autohell])
# AC_CONFIG_MACRO_DIR([tmp-autohell])
# AM_INIT_AUTOMAKE([foreign])
# AM_INIT_AUTOMAKE([1.9 subdir-objects])

# use the C++ compiler for the following checks
AC_LANG([C++])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# distribute additional compiler and linker flags
# --> set these variables instead of CXXFLAGS or LDFLAGS
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([LIBS])

# files to generate via autotools (.am or .in source files)
AC_CONFIG_FILES([Makefile])

# generate the final Makefile etc.
AC_OUTPUT
