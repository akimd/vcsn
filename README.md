Vcsn
====

[Vcsn](http://vcsn.lrde.epita.fr) is a platform for weighted automata and
rational expressions.

It consists of an efficient C++ generic library, shell tools, Python
bindings, and a graphical interactive environment on top of IPython.

Copyright (C) 2012-2016 The Vaucanson Group.

Overview
--------

Initiated by Jacques Sakarovitch and Sylvain Lombardy in 2000, Vcsn is the
result of a collaboration between [Télécom
ParisTech](http://www.telecom-paristech.fr) and the [EPITA Research and
Development Laboratory (LRDE)](http://www.lrde.epita.fr).

The goal of this platform is to enable the development of C++ programs
manipulating weighted finite automata in an abstract and general way with,
at the same time, a large specialization power. On the one hand, we can
write algorithms working on every automaton with weights in any semiring and
with words from any free monoids. And on the other hand, a particular
algorithm can be specialized for a particular data structure.

The Python bindings, and particularly the IPython interface, make Vcsn a
tool particularly well suited for practical sessions in courses of Formal
Language Theory.  More generally, it proves to be a handy means to explore
compositions of algorithms on automata from small sizes to "real world"
cases.

Although it is now quite mature, Vcsn is an ongoing development
project. Therefore some algorithms and data structures may change in the
future.

Please send any question or comments to <vcsn@lrde.epita.fr>, and report
bugs to either our issue tracker
<https://gitlab.lrde.epita.fr/vcsn/vcsn/issues>, or via emails to
<vcsn@lrde.epita.fr>.

Using Vcsn
----------

Documentation about Vcsn can be found in several places:

- the directory `doc/notebooks` documents the Python interface and provides
  examples. It is also helpful to understand the C++ API.  This
  documentation is also available on line. The [online
  notebooks](http://vcsn-sandbox.lrde.epita.fr/tree/Doc) will be frequently
  updated, so please be sure to check there. In particular, be sure to read
  the [introduction to
  Vcsn](http://vcsn-sandbox.lrde.epita.fr/notebooks/Doc/!Read-me-first.ipynb).

- the file `NEWS.md` includes many examples of how to run commands and
    algorithms.

- the directory `tests/python` contains tons of test cases written in
  Python.

- the C++ low-level interface is documented via comments in the header files
  (`vcsn/algos/*.hh`).

- the dyn:: C++ interface is documented in `vcsn/dyn/algos.hh`.

Installation
------------

To install Vcsn on your system, type in the classical sequence at the
command prompt:

    ./configure
    make
    make install (as root)

Do not hesitate to run `make -j3` if, for instance, your CPU features 4
threads.

Note that an installation is specific to the compiler used to install
it. Indeed, the call to `./configure` enables some workarounds and,
consequently, users must compile with the same compiler to avoid
compatibility problems.

Between `make` and `make install`, you may also want to run:

    make check

It run the test suite to check the whole platform. Beware that checking
Vcsn is a very long process, also consider `-j3`.

### Build Requirements

Vcsn was tested with the [GNU Compiler Collection (GCC)](http://gcc.gnu.org)
versions 5, 6 and [Clang](http://clang.llvm.org) 3.5, 3.6, 3.7, 3.8, 3.9.

[Boost](http://www.boost.org) is a C++ library which provides many useful
features.  You must install this library on your system.  Vcsn should
support any version after 1.49.  The following Boost components are used:

- Boost.Algorithm
- Boost.DynamicBitset
- Boost.Filesystem
- Boost.Flyweight
- Boost.Heap
- Boost.Iterator
- Boost.Python
- Boost.Range
- Boost.Regex
- Boost.System
- Boost.Tokenizer

[Ccache](http://ccache.samba.org) saves the user from repeated compilations.

To load plugins, Vcsn relies on libltdl, which is a component of the [GNU
Libtool](http://www.gnu.org/software/libtool/) project.  Depending on your
distribution/packaging system, you may have to install `libltdl-dev` (e.g.,
Debian) or libtool (Mac Ports).

Vcsn uses the Dot format to save automaton in a human readable file. You
should install [Graphviz](http://www.research.att.com/sw/tools/graphviz) to
visualize these `.gv` files.

To provide safe support for ℚ, Vcsn relies on [The GNU Multiple Precision
Arithmetic Library](https://gmplib.org).

[Doxygen](http://doxygen.org) is used to generate the C++ reference
documentation.

### Libraries installed in non-standard directories

If you have installed Boost in a non-standard directory (i.e., a directory
that is not searched by default by your C++ compiler), you will have to set
the `CPPFLAGS` and `LDFLAGS` variables to pass the necessary `-I` and `-L`
options to the preprocessor and linker.

For instance if you installed Boost in `/opt/boost/` you should run
`./configure` as follows:

    ./configure CPPFLAGS="-I/opt/boost" LDFLAGS="-L/opt/boost"

### Layout of the tarball

The project directory layout is as follows:

build-aux
:   Auxiliary tools used by the GNU Build System during `configure` and
    `make` stages.

doc
:   Doxygen documentation, and IPython notebooks.

share
:   Data files to be installed on your system.

lib
:   Various libraries, including instantiation of some contexts.

vcsn
:   The Vcsn C++ Library headers.

python
:   The Python binding.

bin
:   Various programs to install. In particular the program vcsn, which
    provides access to all the other programs. See vcsn --help.

tests
:   The test suites.

Starting from the repository
----------------------------

To contribute to Vcsn, or to build it from its Git repository, you need
more tools:

- Automake 1.14 or newer
- Autoconf 2.69 or newer
- Bison 3.0.4 or newer
- Flex 2.5.35 or newer

Before the configuration steps, run:

    ./bootstrap

to set up the GNU Build system.

License
-------

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

The complete GNU General Public License Notice can be found as the
`COPYING.txt` file in the root directory.

Contacts
--------

The team can be reached by mail at <vcsn@lrde.epita.fr>. Snail mail
addresses follow.

- Vcsn - LRDE

  Akim Demaille & Alexandre Duret-Lutz\
  Laboratoire de Recherche et Développement de l'EPITA\
  14-16 rue Voltaire\
  94276 Le Kremlin-Bicêtre CEDEX\
  France


<!--

LocalWords:  Vcsn Vaucanson Sakarovitch Télécom ParisTech EPITA LRDE automata
LocalWords:  semiring monoids Vcsn's txt vcsn algos hh dyn GCC DynamicBitset
LocalWords:  Regex Tokenizer Ccache libltdl Libtool dev libtool Graphviz gv de
LocalWords:  Doxygen CPPFLAGS LDFLAGS preprocessor IPython instantiation Akim
LocalWords:  Automake Autoconf Demaille Alexandre Duret Lutz Laboratoire et
LocalWords:  Développement l'EPITA Bicêtre CEDEX rst API ipynb ispell
LocalWords:  Sylvain american

Local Variables:
coding: utf-8
ispell-dictionary: "american"
fill-column: 76
mode: markdown
End:

-->
