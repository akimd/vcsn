===========================
 Introduction to Vaucanson
===========================

Vaucanson_, a C++ generic library for weighted finite state machines.

.. _Vaucanson: http://vaucanson.lrde.epita.fr

Vaucanson 2, a generic library for finite state machines.
Copyright (C) 2012-2014 The Vaucanson Group.

.. contents::


Overview
========

Initiated by Jacques Sakarovitch in 2000, Vaucanson is a project
developed by the `Télécom ParisTech`__ and the `EPITA Research and
Development Laboratory (LRDE)`__.

__ http://www.telecom-paristech.fr
__ http://www.lrde.epita.fr

The goal of this platform is to enable the development of C++ programs
manipulating weighted finite automata in an abstract and general way
with, at the same time, a large specialization power.  On the one
hand, we can write algorithms working on every automaton with weights
in any semiring and with words from any free monoids.  And on the
other hand, a particular algorithm can be specialized for a particular
data structure.

Yet, Vaucanson is an ongoing development project.  Therefore
algorithms, data structures and the general architecture are not
totally stable and well tested.

Please send any question or comments to vaucanson@lrde.epita.fr, and
report bugs to either our issue tracker
http://vaucanson.lrde.epita.fr/, or to vaucanson-bugs@lrde.epita.fr.


Using Vaucanson
===============

Vaucanson 2.0 lacks a serious documentation, this is definitely a
weakness.  However:

- the directory share/vcsn/notebooks contains some (sparse)
  documentation, also available on line.  The `online notebooks`_ will
  be frequently updated, so please be sure to check there.

- the file ``NEWS.txt`` includes many examples of how to run commands
  and algorithms.

- the directory tests/python contains tons of test cases written in
  Python.

- the C++ low-level interface is documented via comments in the header
  files (``vcsn/algos/*.hh``).

- the `dyn::` C++ interface is documented in ``vcsn/dyn/algos.hh``.

.. _`on line notebooks`: https://www.lrde.epita.fr/dload/vaucanson/2.0/notebooks/


Installation
============

To install Vaucanson on your system, type in the classical sequence at
the command prompt::

        ./configure
        make
        make install (as root)

Note that an installation is  specific to the compiler used to install
it. Indeed, the call  to ``./configure`` enables some workarounds and,
consequently,  users must  compile  with the  same  compiler to  avoid
compatibility problems.

Between ``make`` and ``make install``, you may also want to run::

        make check

It run the test suite to check the whole platform.


Build Requirements
------------------

Vaucanson was tested with the `GNU Compiler Collection (GCC)`_ version
4.8, 4.9 and Clang++ 3.3, 3.4, and 3.5.

.. _GNU Compiler Collection (GCC): http://gcc.gnu.org/

Vaucanson uses the Dot format to save automaton in a human readable
file.  You should install Graphviz_ to visualize these ``.gv`` files.

.. _Graphviz: http://www.research.att.com/sw/tools/graphviz/

`Boost`_ is a C++ library which provides many useful objects,
including hash tables. Currently, Boost is used in algorithms only,
but its use shall be extended to automata structures and other
portions of code. You must install this library on your system.
Vaucanson should support any version after 1.49.  The following Boost
components are used:

- Boost.Algorithm
- Boost.DynamicBitset
- Boost.Flyweight
- Boost.Heap
- Boost.Iterator
- Boost.Python
- Boost.Range
- Boost.Regex
- Boost.Tokenizer

.. _Boost: http://www.boost.org/


Libraries installed in non-standard directories
-----------------------------------------------

If you have installed Boost in a non-standard directory (i.e., a
directory that is not searched by default by your C++ compiler), you
will have to set the ``CPPFLAGS`` and ``LDFLAGS`` variables to pass
the necessary ``-I`` and ``-L`` options to the preprocessor and
linker.

For instance if you installed Boost in ``/opt/boost/`` you should run
``./configure`` as follows::

        ./configure CPPFLAGS="-I/opt/boost" LDFLAGS="-L/opt/boost"


Layout of the tarball
---------------------

The Vaucanson project directory layout is as follows:

build-aux
   Auxiliary tools used by the GNU Build System during ``configure``
   and ``make`` stages.

share
   Data files to be installed on your system.  They include
   example automata and notebooks.

lib
   Various libraries, including instantiation of some contexts.

vcsn
   The Vaucanson C++ Library headers.

python
   The Python binding.

bin
   TAF-Kit sources.

tests
   The test suites.


Starting from the repository
============================

To contribute to Vaucanson 2, or to build it from its Git repository,
you need more tools:

- Automake 1.14 or newer
- Autoconf 2.69 or newer
- Bison 3.0 or newer
- Flex 2.5.35 or newer


Licence
=======

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

The complete GNU General Public Licence Notice can be found as the
``COPYING.txt`` file in the root directory.

The Vaucanson Group consists of people listed in the ``AUTHORS.txt`` file.



Contacts
========

The team can be reached by mail at vaucanson@lrde.epita.fr. Snail mail
addresses follow.

* Vaucanson - Télécom Paristech

  | Jacques Sakarovitch
  | Télécom Paristech
  | 46, rue Barrault
  | 75634 Paris CEDEX 13
  | France

* Vaucanson - LaBRI

  | Sylvain Lombardy
  | Laboratoire Bordelais de Recherche en Informatique
  | 351, cours de la Libération
  | 33405 Talence Cedex
  | France

* Vaucanson - LRDE

  | Akim Demaille & Alexandre Duret-Lutz
  | Laboratoire de Recherche et Développement de l'EPITA
  | 14-16 rue Voltaire
  | 94276 Le Kremlin-Bicêtre CEDEX
  | France

.. Local Variables:
.. mode: rst
.. End:
