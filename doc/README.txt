* To add a new algorithm "foo" in the three layers
** vcsn/algos/foo.hh
Implement the algorithm, but also its dyn/static bridge.  Be sure to
use the right syntax so that the registries-gen tool works as expected.

Add it to the repo.

** vcsn/local.mk
Ship the file.
Respect/improve the alphabetical order.

** vcsn/ctx/instantiate.hh
Place the instantiation of your algorithm where it belongs if you
think it should be created by default.

** vcsn/dyn/algos.hh
Declare the existence of dyn::foo with the proper interface (the one
specified in vcsn/algos/foo.hh via REGISTRY_DECLARE).
Respect/improve the alphabetical order.

** lib/vcsn/algos/others.cc
Algorithms with a simple signature, doing nothing fancy, will be taken
care of automatically.  If you need something specific, do in
lib/vcsn/algos/others.cc.  Take inspiration from the other algorithms.

** lib/vcsn/local.mk
If you created lib/vcsn/algos/foo.cc, declare it here.

** bin/vcsn-foo.cc
Have a look at other commands with similar signatures to see what's to
do.
Add it to the repo.

** bin/local.mk.
Adjust all_vcsn and %C%_vcsn_foo_LDADD.  Respect the order.

** tests/tafkit/foo.chk
See how other files are written.
Add it to the repo.

** tests/tafkit/local.mk
Adjust.  Pay extra attention to the handle of foo.dir files if you
used external files.  See how similar cases are treated.

* To add a new pre-built context "foo"
** create a new vcsn/ctx/foo.hh file
** register it in vcsn/local.mk
alphabetical order

** register it in lib/local.mk
pkglib_LTLIBRARIES += lib/libfoo.la
lib_libfoo_la_SOURCES = lib/ctx/libctx.cc
lib_libfoo_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=foo

* Naming convensions
The short names below applies to local variables.  Use meaningful (not
hyper-short) names for long-lived entities (e.g., function names,
members, etc.).

automaton: aut, aut_      Aut, Automaton
LabelSet: ls, labelset,   LabelSet
GenSet: gs                GenSet
context: ctx              Context, Ctx
WeightSet: ws, weightset  WeightSet
Weight/word: w
label: l
state: s
transition: t

ostream: os, o
istream: is

The print functions take the printee first, then the stream.

Local Variables:
mode: outline
End:
