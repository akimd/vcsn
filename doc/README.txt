* To add a new algorithm "foo" in the three layers
** vcsn/algos/foo.hh
Implement the algorithm, but also its dyn/static bridge.
Add it to the repo.

** vcsn/local.mk
Ship the file.
Respect/improve the alphabetical order.

** vcsn/ctx/instantiate.hh
Place the instantiation of your algorithm where it belongs.

** vcsn/dyn/algos.hh
Declare the existence of dyn::foo with the proper interface (the one
specified in vcsn/algos/foo.hh via REGISTER_DECLARE).
Respect/improve the alphabetical order.

** lib/vcsn/algos/foo.cc
Implement the registry.  It might be sensible to use an existing file
instead of create a bazillion of such small files.  For instance
is_accessible, is_coaccessible etc. live in the same file.

Add it to the repo.

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

* Environment variables
** VCSN_DEBUG
Improve the display with debugging information.
*** dot
display in parens the real state numbers.
*** is_ambigious
display the couple of states which is outside the diagonal.
*** proper
Read VCSN_DEBUG as an integer specifying the level of details to dump.

** VCSN_DYN
Display information about registration and query about dyn algorithms.

** VCSN_ITERATIVE
Specify that "power" should perform the naive iterative multiplicative
approach, instead of the squaring one.

** VCSN_ORIGINS
Dump on stdout the "origins" of states for algorithms that build
automata from sets of states of input automata or other types of
information (e.g., derived-term, product, determinize, minimize,
shuffle, infiltration...).

Use this Perl snippet to put turn this information in something that
Graphviz displays.

  # Honor origins if there are some.
  # Remove "orientation = landscape" from OpenFST, it breaks OS X's
  # Graphviz rendering.
  perl -pi -0777 -e '
      s{/\* Origins\.\n(.*?\n)\*/\n}{ $orig = $1; ""; }gmse;
      s{^    node \[shape = circle\]\n( {4}.*?\n)*}{$orig}gms
        if $orig;
      s{^(orientation = Landscape;)$}{/* teedot: $1 */}m;
    ' $file

** VCSN_BINARY
Force ratexpset to build AST variadic nodes with only two children.

** VCSN_PARENS
Force the display of useless parentheses.

** VCSN_SEED
Disable the generation of a random seed, stick to the compile-time
default seed.

** VERBOSE
Make the test suite more verbose.

** YYDEBUG, YYSCAN
Set to enable Bison parser/ Flex scanner tracing.  Can be an integer
to denote nesting (which is useful for instance for dot parsing which
can fire ratexp parsing: specify how many layers you want to make
verbose).

Local Variables:
mode: outline
End:
