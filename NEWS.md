![Vcsn Logo](share/vcsn/figs/vcsn.png)

Vcsn Release Notes
==================

This file describes user visible changes in the course of the development of
Vcsn, in reverse chronological order.  On occasions, significant changes in
the internal API may also be documented.

# Vcsn 2.8 (2018-05-08)

We are happy to announce the release of Vcsn 2.8, a bug fix release.

For more information see the detailed news below.

## New Features
### vcsn diagnose: check that Vcsn is properly installed
This tool runs a few commands to check that Vcsn works properly.  It
generates diagnostics that help the Vcsn team understand problems on the
user side.

## Bug Fixes
### Spontaneous transitions in literal automata
In literal automata, transitions without explicit label were accepted but
misinterpreted.  For instance in this automaton in Daut:

    context = lao, q
    $ 0 <2>
    0 1 <3>
    1 $ <4>

the pre- and post-transitions have no label, but are weighted (by 2 and 4).
The inner transition between states 0 and 1 should be labeled with `\e` (and
be weighted by 3).  Unfortunately the user was expected to make the `\e`
explicit:

    context = lao, q
    $ 0 <2>
    0 1 <3>\e
    1 $ <4>

otherwise unexpected results would be yielded (for instance the first
automaton was considered proper).

Now, missing labels are interpreted as the empty word, and both
specifications above denote the same automaton.

----------------------------------------------------------------------

# Vcsn 2.7 (2018-03-25)

We are happy to announce the release of Vcsn 2.7.  This is mostly a bug fix
release, with improvements in the documentation, based on user feedback.
Most of our efforts are currently devoted to Vcsn 3.0.

For more information see the detailed news below.

## New features
### Improved compatibility between single- and multitape expressions
The automatic promotion from single-tape to multitape is more general.  For
instance:

    In [2]: c = vcsn.context('lat<lan, lan>, b')

    In [3]: c.expression('a')
    Out[3]: a|a

    In [4]: c.expression('a*b* @ (ab)*')
    Out[4]: (a|a)*(b|b)*@((a|a)(b|b))*

    In [5]: c.expression('a*b* @ (ab)*').shortest(10)
    Out[5]: \e|\e + ab|ab

### vcsn doc is a new tool
Run `vcsn doc automaton.determinize`, or `vcsn doc Automata`, etc.  The
special shortcuts `vcsn doc` opens the Read-me-first notebook, and `vcsn doc
index` leads to Algorithms, the page that lists the existing documentation
of algorithms (`automata.determinize`, etc.).

### Compressed efsm files
Vcsn can read efsm files compressed with bzip2 and xz.  In some extreme
cases, the xz-compressed file can be 5% of the original file.

The files for the sms2fr demo are now compressed with xz.

### vcsn score has several new options
The command `vcsn score` benchmarks Vcsn.  Its output can be processed with
`vcsn score-compare` to see the trends in performances between versions.

Benchmarks are now numbered, to give a hint of the progress:

    $ vcsn score
    vcsn version: 2.6-085-g6dcae17ef
      1/116  0.25s : a.is_proper()      # a = "", 1000000x
      2/116  0.11s : b.format("text")   # b = [abc] -> B, 100000x
      3/116  0.35s : b.expression(e)    # e = [ab]{20000}, 1000x
    ...
    115/116  0.75s : a.weight_series()  # a = std(a{12000}+<1>[b-z]{12000}), c = [a-z] -> Nmin, 200x
    116/116  0.89s : a.weight_series()  # a = std([a-z]{200}), c = [a-z] -> Z, 10x

The new option `-o`/`--output` allows to specify the output file name.

Better yet: option `-d`/`--dir` specifies the directory in which the score
file is saved; its name will be forged from `git describe`, something like
`v2.5-050-g01dbf326`.  Such names are interpreted by `vcsn score-compare` to
relate the benches to the git commit title.  Both features need that you run
these commands from a git repository of Vcsn.

Option `-j`/`--job` allows to run the benchmarks concurrently.  This can be
very useful to "warm" vcsn (have it compile the needed algorithms), or to
get a nice approximation of the actual benches, however, sticking to a
single bench at a time is recommended to get faithful measurements.

Option `-l`/`--list` lists the benches without running them.

Option `-s`/`--sort` sorts the benchmarks before running them.

### Documentation
Several errors were fixed.  The page `expression.compose.ipynb` is new.

### Examples of C++
The directories `tests/demo` and `tests/benchmarks` contain more examples
using the Vcsn C++ library.

## Bug fixes
### Incorrect order for 8bit characters
Calling `compare` on labels would lead to surprising results with 8bit
characters (seen as negative ints).  This resulted in the incorrect display
of the expression `[\x01-\xfe]` as `[\x80-\xfe] + [\x01-\x7f]`.

Both are fixed, and 1 is less than 254 again.

----------------------------------------------------------------------

# Vcsn 2.6 (2017-11-13)

The Vcsn team is happy to announce the long overdue release of Vcsn 2.6.
Most of our work was devoted to providing a better, smoother, user
experience.  This includes improvements in the build system, better
performances, extended consistency, and more legible error messages.
Multitape expressions also received a lot of attention.

For more information see the detailed news below.

We warmly thank our users who made valuable feedback (read "bug reports"):
Victor Miller, Dominique Soudière and Harald Schilly.  Dominique and Harald
helped integrating Vcsn into CoCalc (formerly SageMathCloud).

People who contributed to this release:

- Akim Demaille
- Clément Démoulins
- Clément Gillard
- Sarasvati Moutoucomarapoulé

## New features
### Promotion from single to multitape
It is now possible to mix single-tape expressions with multitape
expressions.  This is especially handy when using labels classes (`[abc]`).
For instance:

    In [2]: zmin2 = vcsn.context('lat<lan, lan>, zmin')

    In [3]: zmin2.expression(r'([ab] + ⟨1⟩(\e|[ab] + [ab]|\e))*')
    Out[3]: (a|a+b|b+<1>(\e|(a+b)+(a+b)|\e))*

is a easy means to specify an expression generating an automaton that
computes the edit-distance (between words or languages).  Or, with a wide
alphabet:

    In [4]: zmin = vcsn.context('lan(a-z), zmin')

    In [5]: zmin2 = zmin | zmin

    In [6]: zmin2
    Out[6]: {abcdefghijklmnopqrstuvwxyz}? x {abcdefghijklmnopqrstuvwxyz}? -> Zmin

    In [7]: zmin2.expression(r'([^] + ⟨1⟩(\e|[^] + [^]|\e))*')

In the future the display of expressions will also exploit this approach.

### vcsn compile now links with precompiled contexts
If you write a program using dyn::, then it will benefit from all the
precompiled algorithms.

### vcsn compile now supports --debug
Use this to avoid the removal of intermediate object files.  On some
platforms, such as macOS, the object file contains the debug symbols, so
their removal makes the use of a debug harder.

### vcsn compile now uses -rpath
Programs/libraries generated by `vcsn compile` needed to be told where the
Vcsn libraries were installed.  In practice, it meant that `vcsn run` was
needed to execute this program, and in some situations it was not even
sufficient.

Now, `vcsn compile my-prog.cc` generates `my-prog` which can be run directly.

### random_expression supports the tuple operator
It is now possible to generate multitape expressions such as `(a|x*)*`.
Before, random_expression was limited to expressions on multitape labels
such as `(a|x)*`.

    In [1]: import vcsn

    In [2]: c = vcsn.context('lan(abc), q')

    In [3]: c2 = c | c

    In [4]: for i in range(10):
       ...:     e = c2.random_expression('|, +, ., *=.2, w.=.2, w="min=-2, max=2"', length=10)
       ...:     print('{:u}'.format(e))
       ...:
    a|a+(ε|b)*+⟨1/2⟩ε|ε
    (ε+c)a|(⟨2⟩ε+c)
    (ε|b+(c|a)*)*
    ε|a+a|b
    c|b+⟨2⟩ε|(ε+b)
    ε|a+((a|ε)(ε|b)(ε|c))*(b|ε)
    (a|b+c|ε)(a|b)
    ε*|ε
    ε+a|a+⟨2⟩(b|ε)
    (ε+ε*)|ε

Generating expressions with compose operators is also supported.

    In [5]: for i in range(10):
       ...:     e = c2.random_expression('|=.5, +, ., @=2', length=10, identities='none')
       ...:     print('{:u}'.format(e))
       ...:
    ((b|ε)ε)(ε(ε|a))
    ε(ε|c)+((ε|a)(a|b)+ε|c)
    ((ε|a)(ε|b))((b|a)(c|c)@b|b)
    (b|b@ε+a|b)@(c|ε@b|ε)
    (ε+ε|a)@(ε|c@ε|a+b|ε)
    (ε|a+ε|c)((ε|b)(c|ε))
    (a|ε)((ε+c)|(a+ε))
    c|ε@(a|ε)(ε|b@b|ε)
    (ε+ε|b)@ε|b+ε|a
    b(ε²)|((ε+ε)+a)

### New algorithms
A few algorithms were added:

  - context.compose

    Composing `{abc} x {xyz}` with `{xyz} x {ABC}` gives, of course,
    `{abc} x {ABC}`.

  - expansion.normalize, expansion.determinize

    These are rather low-level features.  The latter is used internally when
    derived-term is asked for a deterministic automaton.

  - expansion.expression

    The "projection" of an expansion as an expression.

  - label.compose

    For instance `a|b|x @ x|c` -> `a|b|c`.

  - polynomial.compose

    Additive extension of monomial composition.  For instance composing
    `a|e|x + a|e|y + a|e|\e` with `x|E|A + y|E|A + \e|E|A` gives
    `<3>a|e|E|A`.

  - polynomial.shuffle and polynomial.infiltrate

    The missing siblings of polynomial.conjunction.

### Doxygen is no longer needed
By default, `make install` generated and installed the C++ API documentation
using Doxygen.  It is now disabled by default, pass `--enable-doxygen` to
`configure` to restore it.

## Bug Fixes
### Severe performance regression when reading daut
Version 2.5 introduced a large penalty when reading daut files, especially
large ones.

### Tupling of automata could be wrong on weights
As a consequence, `expression.inductive` was also producing incorrect
automata from multitape expressions.

### Polynomials featuring the zero-label
Under some circumstances it was possible to have polynomials featuring
monomials whose label is the zero of the labelset (e.g., `\z` with
expressions as labels).  This is fixed.

### Portability issues
Newest compilers are now properly supported.

## Changes
### Divisions
The division of expressions and automata now compute the product of the
weights, not their quotient.

### Handling of the compose operator
The support of the compose operators in expansions was completely rewritten.
As a result, the derived-term automata are often much smaller, since many
useless states (non coaccessible) are no longer generated.

For instance the derived-term automaton of `(\e|a)* @ (aa|\e)*` has exactly
two states.  It used to have three additional useless states.

### Better diagnostics
Many error messages have been improved.

The Daut automaton format now treats `->` as a keyword, so `0->1 a` is now
properly read instead of producing a weird error message because Vcsn
thought your state was named `0->1`.

----------------------------------------------------------------------

# Vcsn 2.5 (2017-01-28)

The Vcsners are proud to announce the release of Vcsn 2.5, aka the
k-lightest release!

Noteworthy changes include:

- two new implementations to compute the k-lightest paths (aka "best" paths:
  with the smallest weights) in tropical automata.

- several new demos showing how to use the C++ library, including a
  translator from (French) text messages (aka SMS) into French.

- a new means for the users to configure Vcsn.  This adds a new prerequisite
  to build Vcsn: libyaml-cpp.

- a much better and faster caching system of runtime generated C++ code.
  Users of dyn (and therefore of Python) should note an improved
  amortization of the compilation costs.

- several portability issues reported by users were fixed.

For more information, please, see the detailed news below.

People who worked on this release:

- Akim Demaille
- Clément Démoulins
- Clément Gillard
- Sarasvati Moutoucomarapoulé
- Sébastien Piat
- Younes Khoudli

## 2017-01-23
### Improved error messages
Parse errors were improved with caret-style reports for expressions and
automata in dot (Graphviz) and daut syntax.

    In [5]: vcsn.Q.expression('<1/2>a + <1/0>b')
    RuntimeError: 1.10-14: Q: null denominator
    <1/2>a + <1/0>b
             ^^^^^
      while reading expression: <1/2>a + <1/0>b

    In [6]: vcsn.automaton('''
       ...: $ -> 0
       ...: 0 -> 1 <1/2>a, b
       ...: 1 -> $''')
    RuntimeError: 3.1-16: B: unexpected trailing characters: /2
      while reading: 1/2
      while reading: <1/2>a, b
    0 -> 1 <1/2>a, b
    ^^^^^^^^^^^^^^^^
      while reading automaton

    In [7]: vcsn.automaton('digraph{vcsn_context="lal, b" 0->1[label="<2>a"]}')
    RuntimeError: 1.35-48: B: invalid value: 2
      while reading: 2
      while reading: <2>a
    digraph{vcsn_context="lal, b" 0->1[label="<2>a"]}
                                      ^^^^^^^^^^^^^^
      while reading automaton


## 2017-01-14
### "auto" automaton file format
Pass "auto" to read_automaton (in dyn, static, Python or the Tools) to let
the system guess the automaton file format (daut, dot, etc.).

## 2017-01-11
### Sms2fr
New Natural Language Processing demonstration of the computations of the
lightest paths. This application is a translator from SMS (i.e., text messages)
to proper French.  The implementation is based on _Rewriting the orthography of
SMS messages_, François Yvon, In Natural Language Engineering, volume 16, 2010.
The translator uses pre-trained automata and through compositions with the
automaton representing the text message, generates all possible translations of
the word. The best solution is then found with a shortest path algorithm.
An example is given in the `Sms2fr.ipynb` notebook.

## 2017-01-01
### A richer dyn
The `vcsn::dyn` API was enriched.  All the dyn types now support the usual
operators: comparisons (`==`, `!=`, `<`, `<=`, `>`, `>=`), and compositions
(`+`, `*`, `&`).  New functions facilitate the creation of `dyn` values from
strings (`make_word`, etc.).  The file `tests/demos/operators.cc` shows
several examples, explained in the `C++-Library.ipynb` notebook.  For
instance:

    // A simple automaton.
    auto a1 = make_automaton("context = lal, q\n"
                             "$ 0 <1/2>\n"
                             "0 1 <2>a, <6>b\n"
                             "1 $\n", "daut");
    // Its context.
    auto ctx = context_of(a1);

    // Evaluate it.
    assert(evaluate(a1, make_word(ctx, "a")) == make_weight(ctx, "1"));
    assert(evaluate(a1, make_word(ctx, "b")) == make_weight(ctx, "3"));

    // Concatenate to itself.
    auto a2 = a1 * a1;
    assert(evaluate(a2, make_word(ctx, "ab")) == make_weight(ctx, "3"));

    // Self-conjunction, aka "power 2".
    auto a3 = a1 & a1;
    assert(evaluate(a3, make_word(ctx, "b")) == make_weight(ctx, "9"));

## 2016-12-25
### Configuration
Vcsn now supports configuration files.  They will be used to provide users
with a means to customize Vcsn, for instance to tune the graphical rendering
of the automata, to tailor the display of expressions, etc.

For a start, it provides a simple means to get the configuration
information, including from Vcsn Tools.

    $ vcsn ipython --no-banner
    In [1]: import vcsn

    In [2]: vcsn.config('configuration.cxxflags')
    Out[2]: '-Qunused-arguments -O3 -g -std=c++1z'

    $ vcsn configuration configuration.cxxflags
    -Qunused-arguments -O3 -g -std=c++1z

This adds a new dependency: libyaml-cpp.  Beware that version 0.5.2 is buggy
and will not work properly.  Use 0.5.1, or 0.5.3 or more recent.

## 2016-12-20
### compare: new algorithm
Three-way comparison is now available in all the layers, as `compare`, for
automata, expressions, labels, polynomials and weights.  This is used in
Python to implement the comparisons (`<`, `<=`, `>`, `=>`, `==`, `!=`) of
expressions, and of automata (`<`, `<=`, `>`, `=>`).

However, since the comparison on automata is performed on the _list_ of
transitions, automata that are "very much alike" (i.e., different only by
superficial details) will be considered different, so `==` and `!=` are
still using a safer, but much slower, comparison.

    In [2]: exp = vcsn.Q.expression

    In [3]: exp('a').compare(exp('b'))
    Out[3]: -1

    In [4]: exp('a').compare(exp('a'))
    Out[4]: 0

    In [5]: exp('<3>a').compare(exp('a'))
    Out[5]: 1

## 2016-12-12
### k shortest paths algorithms
Two algorithms for searching k shortest paths in graphs have been implemented:
"Eppstein" and "Yen".  Hence, we can now search for the k lightest (smallest
with respect to weights, aka "best" paths) paths in an automaton through the
"lightest" algorithm.
"lightest" used to compute these paths with an inefficient heap-based implementation:

    aut.lightest(5)
    aut.lightest(5, "auto")

Note that "auto" does not use the latter algorithm when returning only one path,
as simpler shortest path algorithms would apply to the situation and be more
efficient. It then uses Dijkstra's algorithm.
It can now be called with the given Yen and Eppstein algorithms as follows:

    aut.lightest(5, "eppstein")
    aut.lightest(5, "yen")

Yen's algorithm requires the given automaton to have no cycles, while Eppstein
supports any type of automaton. For small values of k, Yen's algorithm has better
performances than Eppstein, but with increasing values of k, Eppstein is always
more efficient.


----------------------------------------------------------------------

# Vcsn 2.4 (2016-11-16)

The Vcsn team is happy to announce the release of Vcsn 2.4, code-named "the
quotient tools"!

Noteworthy changes include, besides a few bug fixes:

- an overhaul of the "Vcsn Tools" (previously known as TAF-Kit).  Because
  the tools are now automatically generated, they are much more extensive
  than previously: (almost) all of the dyn algorithms are now available from
  the shell.  It now also supports the 'daut' format for automata.

    $ vcsn thompson -Ee '[ab]*c' | vcsn proper | vcsn determinize | vcsn minimize | vcsn to-expression
    (a+b)*c

    $ vcsn random-expression -C 'lal(abc), z' '+, ., w., length=20, w="min=-2, max=10"'
    (a+<2>(ac)+<5>(acca))a

- an new method to construct an automaton from an extended expression:
  `expression.inductive`.  This provides an alternative to
  `expression.derived_term`.  Currently provides a single flavor: generation
  of standard automata.

    In [2]: vcsn.B.expression('! [ab]*a[ab]*').inductive().expression()
    Out[2]: \e+bb*

    In [3]: vcsn.B.expression('! [ab]*a[ab]*').automaton().expression()
    Out[3]: b*

- full support for quotient operators on all entities: labels, expressions,
  automata, expansions, etc.

    In [2]: c = vcsn.context('lan, q')
       ...: c
    Out[2]: {...}? -> Q

    In [3]: label = vcsn.context('law, q').label
       ...: label('abc') / label('c')
    Out[3]: ab

    In [4]: exp = c.expression

    In [5]: exp('ab').ldivide(exp('ab*c'))
    Out[5]: ab{\}ab*c

    In [6]: e = exp('ab').ldivide(exp('ab*c'))
       ...: e
    Out[6]: ab{\}ab*c

    In [7]: e.automaton().expression()
    Out[7]: b*c

  Operators {\} (left quotient) and {/} (right quotient) are available in
  the rational expressions:

    In [8]: e = exp('ab {\} abc*')
       ...: e
    Out[8]: ab{\}abc*

    In [9]: e.expansion()
    Out[9]: \e.[b{\}bc*]

    In [10]: e.derived_term().expression()
    Out[10]: c*

    In [11]: e.inductive().expression()
    Out[11]: \e+cc*

- `automaton.evaluate` works properly on non-free automata, including
  multitape automata:

    In [2]: c = vcsn.context('lan(a-z), nmin')
            a = (c|c).levenshtein()
            a('foo|bar')
    Out[2]: 3

- input/output support for FAdo's format for transducers, and improved
  compatibility with OpenFST.

For more information, please, see the detailed news below.

People who worked on this release:

- Akim Demaille
- Clément Gillard
- Lucien Boillod
- Sarasvati Moutoucomarapoulé
- Sébastien Piat
- Younes Khoudli

People who have influenced this release:

- Alexandre Duret-Lutz
- Jacques Sakarovitch
- Luca Saiu
- Sylvain Lombardy

## 2016-11-04
### random_automaton now generates weights

`context.random_automaton` now takes an optional `weights` parameter,
allowing to set how the weights are generated. The syntax is the same
as the `param` string of `random_weights`.

    In [1]: import vcsn
            ctx = vcsn.context('lal_char(ab), z')
            a = ctx.random_automaton(3, weights='min=0, max=20')
            print(a.format('daut'))
    context = letterset<char_letters(ab)>, z
    $ -> 0
    0 -> $
    0 -> 2 <17>b
    1 -> 1 <13>b
    1 -> 2 <11>b
    2 -> 0 <18>a, <13>b
    2 -> 1 <12>a

## 2016-10-31
### eval is renamed evaluate
For consistency with the remainder of the API, we use the full,
unabbreviated, name: evaluate.

## 2016-10-18
### weight_zero and weight_one are now available in Python
These methods return the "zero" and "one" weights of a context.

    In [1]: import vcsn
            ctx = vcsn.context('lal_char, zmin')
    In [2]: ctx.weight_one()
    Out[2]: 0

    In [3]: ctx.weight_zero()
    Out[3]: ∞

## 2016-10-17
### Left and right divisions are now supported on labels
It is now possible to call left and right divisions on labels from Python,
using `//` and `/` operators (respectively).

    In [1]: import vcsn
            ctx = vcsn.context('law_char, b')
    In [2]: l = ctx.label('a')
            r = ctx.label('abc')
            l // r # == l.ldivide(r)
    Out[2]: bc

    In [3]: l = ctx.label('abc')
            r = ctx.label('bc')
            l / r # == l.rdivide(r)
    Out[3]: a

## 2016-10-11
### TAF-Kit is replaced by Tools
The new command line interface is now automatically generated
from the algorithms in dyn, allowing it to support a lot more
functions than previously.

The supported algorithms are:

    accessible add ambiguous-word are-equivalent are-isomorphic cat
    cerny coaccessible codeterminize cominimize complement complete
    component compose concatenate condense conjugate conjunction
    constant-term context-of copy costandard cotrie de-bruijn
    delay-automaton derivation derived-term determinize difference
    divkbaseb eliminate-state eval expand expression-one expression-zero
    factor focus get-format has-bounded-lag has-lightening-cycle
    has-twins-property identities-of inductive infiltrate insplit
    is-accessible is-ambiguous is-coaccessible is-codeterministic
    is-complete is-costandard is-cycle-ambiguous is-deterministic
    is-empty is-eps-acyclic is-functional is-letterized is-normalized
    is-out-sorted is-partial-identity is-proper is-realtime is-standard
    is-synchronized is-synchronized-by is-synchronizing is-trim
    is-useless is-valid join ladybird ldivide less-than letterize
    levenshtein lgcd lift lightest lightest-automaton lweight
    make-context make-word-context minimize multiply normalize
    num-components num-tapes pair partial-identity prefix project proper
    push-weights quotkbaseb random-automaton
    random-automaton-deterministic random-expression random-weight
    rdivide realtime reduce rweight scc set-format shortest shuffle sort
    split standard star star-height star-normal-form strip subword
    suffix synchronize synchronizing-word thompson to-automaton
    to-expansion to-expression transpose transposition trie trim tuple
    type u universal weight-series zpc

To get more information about a particular algorithm, you can type
`vcsn COMMAND -h` or `--help`:

    $ vcsn eval --help
    usage: vcsn eval [OPTIONS...] [ARGS...]

    Available versions:
     eval: AUT:automaton P:polynomial -> weight
        Evaluate P on AUT.

     eval: AUT:automaton L:word -> weight
        Evaluate L on AUT.

    Try 'vcsn tools --help' for more information.

You can for example generate the Thompson automaton that accepts `ab*`:

    $ vcsn thompson 'ab*' -O daut
    $ -> 0
    0 -> 1 a
    1 -> 4 \e
    2 -> 3 b
    3 -> 2 \e
    3 -> 5 \e
    4 -> 2 \e
    4 -> 5 \e
    5 -> $

For more information, please see the Executables documentation page, and
`vcsn tools -h`.

## 2016-10-04
### FAdo: transducers and comments support
It is now possible to read and produce transducers in FAdo format.  Comments
are also supported in the parser.

    In [1]: a = vcsn.automaton(data='''
            @Transducer 0 2 * 0 # Final * Initial
            0 0 @epsilon 1
            0 0 0 0
            0 1 @epsilon 1
            0 1 1 0
            1 @epsilon 0 2
            1 @epsilon 1 2
            1 0 0 1
            1 1 1 1''')

    In [2]: print(a.format('fado'))
    @Transducer 0 2 * 0
    0 0 @epsilon 1
    0 0 0 0
    0 1 @epsilon 1
    0 1 1 0
    1 @epsilon 0 2
    1 @epsilon 1 2
    1 0 0 1
    1 1 1 1

## 2016-09-27
### daut native parser and producer
Daut is a simplified Dot syntax for automata.  This format was only available
in Python. It is now possible to read and produce it in C++.

    $ vcsn cat -A -I daut -O daut -f lal_char_q.daut
    context = letterset<char_letters(abc)>, q
    $ -> 0 <3>
    0 -> 1 <1/2>a, <1/3>b
    1 -> $ <2>

## 2016-09-21
### Improved compatibility with newer OpenFST
As OpenFST only supports a single initial state, pre is showed in case of
several ones, with spontaneous transitions to them.  Pre was represented by a
very large integer which was read as a negative one in newer version of
OpenFST, thus raising an error.  The state number immediately after the highest
state number is now used.

## 2016-09-19
### automaton.eval supports polynomials
It is now possible to evaluate polynomials of words on automata.

## 2016-09-12
### make_word_context is exposed in Python
It is now possible to call `context.word_context()` to get the context of the
words of any context.

## 2016-09-08
## automaton.eval supports non-free labelsets
It is now possible to evaluate words on automata with non-free labelsets.

For example, we can compute the edit distance between two words:

    In [2]: c = vcsn.context('lan(a-z), nmin')
            a = (c|c).levenshtein()
            a('foo|bar')
    Out[2]: 3

    In [3]: a('bar|baz')
    Out[3]: 1

    In [4]: a('qux|quuux')
    Out[4]: 2

## 2016-07-28
### expression: inductive
Implemented as a hidden feature in Vcsn 2.3, `expression.inductive` is a new
way of constructing automata from expressions, based on the algorithm given
as argument.  The only algorithm implemented yet is "standard" which uses
standard operations to construct a standard automaton. The difference with
`expression.standard` is that it handles extended expressions.

For example, we can compute the automaton equivalent of such expressions with
the inductive method whereas we cannot with the standard one:

    In [2]: vcsn.B.expression('! [ab]*a[ab]*').inductive().expression()
    Out[2]: \e+bb*

### expression.derived_term supports multitape expressions
Vcsn 2.3 already supports multitape expressions with the derived-term
algorithm, but it was restricted to the expansion-based computation.  The
derivative-based computation is now also supported.

This is only a proof of concept: the implementation is more complex and much
slower than the expansion-based approach.

## 2016-07-25
### expression.derivation works on multitape expressions
It is now possible to compute derivatives wrt labels such as `a|x`, `a|\e`
or `\e|x`.  It is however forbidden wrt `\e|\e`.

## 2016-07-23
### automaton.info: levels of detail
Instead of a Boolean argument `detailed`, `info` now features an integer
argument `details`, defaulting to 2.  A new level, 1, contains only basic
information (number of states and transitions).

## 2016-07-19
### expression: partial_identity
The `partial_identity` algorithm is now available on expressions too.

### tuple: applies to contexts
It is now possible from dyn and Python to tuple several contexts.  For
instance `vcsn.B | vcsn.Q` is `lat<lal, lal>, q`.


----------------------------------------------------------------------

# Vcsn 2.3 (2016-07-08)

About four hundred commits and five months after Vcsn 2.2, we are proud to
announce the release of Vcsn 2.3, code-named "the tuple release"!

As usual, many bugs were fixed (some quite old yet unnoticed so far!).
Noteworthy changes include:

- a particular effort was put on the documentation: there are thirty-five
  new documentation pages, and about forty others were improved.

- full support for a "tuple" operator on all entities: expressions,
  polynomials, automata, etc.

    In [13]: aut = lambda e: vcsn.context('lan, q').expression(e).automaton()

    In [14]: a = aut('[ab]*') | aut('x')

    In [15]: a.shortest(6)
    Out[15]: \e|x + a|x + b|x + aa|x + ab|x + ba|x

  It is also available in the rational expressions themselves:

    In [16]: c = vcsn.context('lat<lan, lan>, q'); c
    Out[16]: {...}? x {...}? -> Q

    In [17]: e = c.expression('[ab]*|x'); e
    Out[17]: (a+b)*|x

    In [18]: e.shortest(6)
    Out[18]: \e|x + a|x + b|x + aa|x + ab|x + ba|x

  The derived-term algorithm supports this operator, and generates
  equivalent multitape automata.

- many error messages were improved, to help users understand their
  mistakes.  For instance, instead of

    In [2]: vcsn.Q.expression('a**').derivation('a')
    RuntimeError: q: star: invalid value: 1

  we now display:

    In [2]: vcsn.Q.expression('a**').derivation('a')
    RuntimeError: Q: value is not starrable: 1
      while computing derivative of: a**
                    with respect to: a

- in addition to `%automaton a`, which allows interactive edition of
  automata, the notebooks now feature two new interactive editors:
  `%context c` to edit/create context `c`, and `%expression e` for
  expressions (with an interactive display of the generated automata).

- one may now generate random rational expressions and control the
  operators and their probabilities.

- a lot of code improvement and consistency enforcement, both in C++ and in
  Python.

For more details, please, see the news below.

People who worked on this release:

- Akim Demaille
- Clément Gillard
- Lucien Boillod
- Raoul Billion
- Sébastien Piat
- Thibaud Michaud

People who have influenced this release:

- Alexandre Duret-Lutz
- Jacques Sakarovitch
- Luca Saiu
- Sylvain Lombardy

## 2016-06-28
### Command line executables
The shell tools (formerly known as TAF-Kit) such as `vcsn standard`, `vcsn
determinize`, etc. have finally been documented!  Several issues have been
fixed too.

## 2016-06-27
### expression widget
You can now use `%expression` in the notebook to interactively edit an
expression and its context while seeing the automaton it builds. You are also
able to chose the identities you want to use for the expression, and the automaton
generating algorithm used to render the automaton.

## 2016-06-20
### proper: more consistent signatures
The Python binding of `automaton.proper` was different from the static and
dyn:: versions for no sound reason: instead of a `direction` argument taking
`backward` or `forward`, it had a `backward` argument taking an Boolean,
and the `prune` and `direction` (well, `backward`) arguments were swapped.

The signature in Python is now consistent:
`aut.proper(direction="backward", prune=False, algo="auto", lazy=False)`.

## 2016-06-16
### renamings
The following operations have been renamed, in all APIs (vcsn::, dyn::,
Python, and TAF-Kit), and on all applicable types (automaton, expansion,
expression, polynomial, weight).

    infiltration -> infiltrate
    ldiv         -> ldivide
    left_mult    -> lweight
    rdiv         -> rdivide
    right_mult   -> rweight
    sum          -> add

## 2016-06-07
### random_automaton: new name for `random`
Random generation of automata is now named `random_automaton`.

## 2016-05-27
### polynomials: more basic operations
Polynomials now support (in the three layers) the addition, multiplication,
exterior products, and conjunction.

    In [4]: p = vcsn.context('law, q').polynomial

    In [5]: p0 = p('<2>a + <3>b + <4>c'); p1 = p('<5>a + <6>b + <7>d')

    In [6]: p0 + p1
    Out[6]: <7>a + <9>b + <4>c + <7>d

    In [7]: p0 * p1
    Out[7]: <10>aa + <12>ab + <14>ad + <15>ba + <18>bb + <21>bd + <20>ca + <24>cb + <28>cd

    In [8]: p0 * 2
    Out[8]: <4>a + <6>b + <8>c

    In [9]: 2 * p0
    Out[9]: <4>a + <6>b + <8>c

    In [10]: p0 & p1
    Out[10]: <10>a + <18>b

## 2016-05-25
### tuple: applies to more types
The Cartesian product, dubbed "tuple" in Vcsn, was already available on
expansions and expressions in the three layers (static, dyn, Python as `|`).
It is now also available on automata and on polynomials.

    In [2]: exp = vcsn.context('lan, q').expression

    In [3]: a = exp('(<2>a)*').automaton()
    In [4]: b = exp('x+y').automaton()

    In [5]: (a|b).shortest(8)
    Out[5]: \e|x + \e|y + <2>a|x + <2>a|y + <4>aa|x + <4>aa|y + <8>aaa|x + <8>aaa|y

    In [6]: a.shortest(4) | b.shortest(2)
    Out[6]: \e|x + \e|y + <2>a|x + <2>a|y + <4>aa|x + <4>aa|y + <8>aaa|x + <8>aaa|y

## 2016-05-20
### quotkbaseb: new algorithm
From a context, a divisor k, and a base b, gives a transducer that,
when given a number in b divisible by k, outputs the quotient of the division
of that number by k in b.

    In [2]: c = vcsn.context('lat<lal_char(0-9), lal_char(0-9)>, b')

    In [3]: c.quotkbaseb(3, 2).shortest(10)
    Out[3]: \e|\e + 0|0 + 00|00 + 11|01 + 000|000 + 011|001 + 110|010 + 0000|0000 + 0011|0001 + 0110|0010

    In [4]: c.quotkbaseb(7, 10).shortest(10)
    Out[4]: \e|\e + 0|0 + 7|1 + 00|00 + 07|01 + 14|02 + 21|03 + 28|04 + 35|05 + 42|06

## 2016-05-02
### expansions: support for &
In addition to automata, expressions and polynomials, the conjunction can
now be performed on expansions.

## 2016-04-30
### sum: optimized version for deterministic automata
The sum of deterministic Boolean automata is now based on a synchronized
product-like approach.

### dyn: vast overhaul and factoring
The implementation of dyn:: values was revised and factored.  No visible
change.

## 2016-04-14
### random_expression
One may now generate random expressions.

    In [2]: rand = vcsn.context('lal(abc), b').random_expression

    In [3]: rand('+,.', length=10)
    Out[3]: c(b+c+ca)c

    In [4]: rand('+,.', length=10)
    Out[4]: c+abcac

    In [5]: rand('+=1, .=1, *=.2', length=10)
    Out[5]: (b(a+b+c))*

    In [6]: rand('+=1, .=1, *=.2', length=10)
    Out[6]: bb*ca

    In [7]: rand('+=1, .=1, *=.2, &=1', length=10)
    Out[7]: c

    In [8]: rand('+=1, .=1, *=.2, &=1', length=10)
    Out[8]: a*cb*

    In [9]: rand('+=1, .=1, *=.2, &=1', length=10)
    Out[9]: a((a+a*)&b*)*

## 2016-03-29
### project: support for expressions and expansions
One may now project (multitape) automata, contexts, expansions, expressions,
labels and polynomials.

## 2016-03-15
### expression: a dot output
Expressions now feature a `"dot"` format to display graphically the structure
of the expression.  There are actually two flavors: `"dot,logical"` (the
default) which shows the semantic tree, and `"dot,physical"` which shows
the DAG that is used to implement the expression (i.e., nodes used multiple
times are displayed only once).

Under IPython, experiment `exp.SVG()` (logical) and `exp.SVG(True)` (physical).

### Python: __format__
The Python objects now support the `format` protocol.  For instance:

    In [2]: c = vcsn.context('lal(abc), q')

    In [3]: for i in range(4):
                e = c.expression('[ab]*a[ab]{{{i}}}'.format(i=i))
                print('{i:3d} | {e:t:20} | {e:u:20} | {n:3d}'
                      .format(i=i, e=e,
                              n=e.automaton().determinize().state_number()))
      0 | (a+b)*a              | (a+b)*a              |   2
      1 | (a+b)*a(a+b)         | (a+b)*a(a+b)         |   4
      2 | (a+b)*a(a+b){2}      | (a+b)*a(a+b)²        |   8
      3 | (a+b)*a(a+b){3}      | (a+b)*a(a+b)³        |  16

## 2016-03-08
### ldiv, rdiv: compute quotient between automata
`automaton.ldiv` and `automaton.rdiv` compute the left and right quotients of
two automata.

    In [1]: import vcsn
            ctx = vcsn.context('lal_char, b')
            aut = lambda e: ctx.expression(e).automaton()

    In [2]: aut('ab').ldiv(aut('abcd')).expression()
    Out[2]: cd

    In [3]: (aut('abcd') / (aut('cd')).expression()
    Out[3]: ab


----------------------------------------------------------------------

# Vcsn 2.2 (2016-02-19)

We are very happy to announce the release of Vcsn 2.2!  This version,
code-named "the lazy release", concludes the work from Antoine and Valentin,
who left EPITA for their final internship.

In addition to the usual load of improvements (more doc and less bugs), this
version features some noteworthy changes:

- several algorithms now offer a lazy variant: compose, conjunction,
  derived_term, determinize, insplit, and proper.  Instead of completing the
  construction on invocation, the result is built incrementally, on demand,
  e.g., when requested by an evaluation.

  This is especially useful for large computations a fraction of which is
  actually needed (e.g., composition of two large automata and then with a
  small one), or for computations that would not terminate (e.g.,
  determinization of some weighted automata).

- the functions `automaton.lightest` and `automaton.lightest_automaton`
  explore the computations (i.e., paths of accepted words) with the smallest
  weights (dubbed "shortest paths" for tropical-min semirings).  They
  feature several implementations controlled via the `algo` argument.

- rational expressions now support UTF-8 operators in input and output.
  They also learned a few tricks to be better looking (e.g., `aaa` => `a³`).

- several new algorithms or improvements or generalizations of existing ones.

- a number of performance improvements.

Please, see the details below.

People who worked on this release:

- Akim Demaille
- Antoine Pietri
- Lucien Boillod
- Nicolas Barray
- Raoul Billion
- Sébastien Piat
- Thibaud Michaud
- Valentin Tolmer

People who have influenced this release:

- Alexandre Duret-Lutz
- Jacques Sakarovitch
- Luca Saiu
- Sylvain Lombardy

## 2016-02-13
### operations on automata: deterministic versions
In addition to the `"general"`, `"standard"` and `"auto"` variants,
multiply, sum and star now support a `"deterministic"` variant, which
guarantees a deterministic result.

Beware that with infinite semirings, some (deterministic) operations might
not terminate.

## 2016-02-05
### conjugate: new algorithm
The `automaton.conjugate` function builds an automaton whose language is the
set of conjugates (i.e., "rotations", or "circular permutations") of the
words accepted by the input automaton.

    In [3]: vcsn.context('lan_char, b')         \
                .expression('(ab)*')            \
                .automaton()                    \
                .conjugate()                    \
                .expression()
    Out[3]: (ab)*+(ab)*{2}+(ba)*ba(ba)*

### demangle: add a gdb pretty-printer for Vcsn types
`vcsn gdb` runs gdb with a pretty-printer, which will automatically Vcsn's
`demangle()` function when a variable with a vcsn type is printed.  Symbols
are then *much* easier to read.

### insplit: support for on-the-fly construction
The implementation of `insplit` now supports a `lazy` argument. This is
especially useful when composing a small transducer with a big one, to avoid
computing the insplit of the right transducer for many states, that are not
going to be useful.

It is used in the composition algorithm for this very reason.

### compose: support for on-the-fly construction
The implementation of `compose` now supports a `lazy` argument. This is
especially useful when composing two big transducers, and then compose with
several small ones. The result of the first composition, although huge, will
never be completely computed, but the required states are computed and cached.

## 2016-01-28
### determinize: support for on-the-fly construction
The implementation of `determinize` now supports a `lazy` argument.  This is
especially useful when working on automata that do not admit a (finite)
deterministic automaton.

For instance:

    In [2]: a = vcsn.Z.expression('a*+(<2>a)*').automaton().determinize(lazy=True)

    In [3]: print(a.as_boxart())
        ╭───╮
    ──> │ 0 │
        ╰───╯

    In [4]: a('')
    Out[4]: 2

    In [5]: print(a.as_boxart())
        ╭───╮  a   ╭─────────╮
    ──> │ 0 │ ───> │ 1, ⟨2⟩2 │
        ╰───╯      ╰─────────╯
          │
          │ ⟨2⟩
          ∨

    In [6]: a('aa')
    Out[6]: 5

    In [7]: print(a.as_boxart())
        ╭───╮  a   ╭─────────╮  a   ╭─────────╮  a   ╭─────────╮
    ──> │ 0 │ ───> │ 1, ⟨2⟩2 │ ───> │ 1, ⟨4⟩2 │ ───> │ 1, ⟨8⟩2 │
        ╰───╯      ╰─────────╯      ╰─────────╯      ╰─────────╯
          │            │                │
          │ ⟨2⟩        │ ⟨3⟩            │ ⟨5⟩
          ∨            ∨                ∨

## 2016-01-14
### lightest: algorithms for "shorter paths"
The `automaton.lightest` looks for words (possibly several) whose evaluation
is the smallest one in the automaton.  In the case of ℕmin and other
tropical semiring, this is often referred to as "shortest paths", but it
applies to other semirings as well.  It features the same interface as
`automaton.shortest` (which looks for shortest accepted words), but offers
several variants, such as `"dijkstra"`, `"bellman-ford"`, `"auto"`...

The `automaton.lightest_automaton` algorithm returns a slice of the
automaton corresponding to the evaluation with the small weight.  It also
offers several variants.

## 2016-01-09
### derived-term: support for on-the-fly construction
The implementation of `derived_term` now supports a `lazy` argument.  This
is especially useful when working on "infinite derived-term automata" (which
happens when requesting a deterministic automaton, or when using the
complement operator).

For instance:

    In [2]: a = vcsn.Z.expression('a*+(<2>a)*').derived_term(deterministic=True, lazy=True)

    In [3]: print(a.as_boxart())
         ╭────────────╮
     ──> │ a*+(⟨2⟩a)* │
         ╰────────────╯

    In [4]: a('')
    Out[4]: 2

    In [5]: print(a.as_boxart())
        ╭────────────╮  a   ╭───────────────╮
    ──> │ a*+(⟨2⟩a)* │ ───> │ a*+⟨2⟩(⟨2⟩a)* │
        ╰────────────╯      ╰───────────────╯
          │
          │ ⟨2⟩
          ∨

    In [6]: a('aa')
    Out[6]: 5

    In [7]: print(a.as_boxart())
        ╭────────────╮  a   ╭───────────────╮  a   ╭───────────────╮  a   ╭───────────────╮
    ──> │ a*+(⟨2⟩a)* │ ───> │ a*+⟨2⟩(⟨2⟩a)* │ ───> │ a*+⟨4⟩(⟨2⟩a)* │ ───> │ a*+⟨8⟩(⟨2⟩a)* │
        ╰────────────╯      ╰───────────────╯      ╰───────────────╯      ╰───────────────╯
          │                      │                      │
          │ ⟨2⟩                  │ ⟨3⟩                  │ ⟨5⟩
          ∨                      ∨                      ∨

## 2015-12-31
### project is available on more structures
One may now project not only multitape automata, but also contexts, labels
and polynomials.

## 2015-12-23
### has-lightening-cycle: new name for `has-negative-cycle`
In static, dyn:: and Python, `has-negative-cycle` is renamed
`has-lightening-cycle`. It makes more sense as we consider `(<1/2>a)*` to be a
lightening cycle.

## 2015-12-21
### Operators on expressions and expansions
The `dyn::complement` function is now available on expansions, in addition
to expressions and automata.  It is bound to the prefix `~` operator in
Python.  The `dyn::tuple` function is available for expressions and
expansions.  It is bound to `|` in Python.

## 2015-12-10
### Exponents in expressions
In addition to "ASCII exponents" in input (e.g., `(ab){4}`), we now support
them in output, possibly in UTF-8:

    In [7]: vcsn.B.expression('aa{2}a²')
    Out[7]: a{5}
    In [8]: print(vcsn.B.expression('aa{2}a²').format('utf8'))
    a⁵

This is especially nice in derived-term automata.

### Tag based dispatch in vcsn::
The treatments for which several algorithms exist (e.g., minimize/cominimize
---hopcropft, moore, weighted, signature, brzozowski, auto---,
determinize/codeterminize ---boolean, weighted, auto---, sum/multiply/star
---standard, general, auto---, etc.) now offer a cleaner tag-based interface
in vcsn::, the static library.  For instance, instead of:

    auto m = minimize_moore(a);

write:

    auto m = minimize(a, moore_tag{});

### dot format
The HTML/XML style strings are now properly supported.

### oneset and proper
The oneset labelset has a single label: one.  It is used to denote automata
with spontaneous transitions only.  Applying proper on such automata
resulted in an ill-formed automaton (with a single subliminal transition
from pre to post).  This is now fixed.

### Flex 2.6
We are now compatible with the new Flex, which made backward incompatible
changes in its API.  Previous versions are supported too.

### Text format
When displaying value sets, the `text` format was improved, and the new
format `utf8` improves on top of it.  An new format name `sname`, replaces
previous uses of `text`:

    In [2]: c = vcsn.context('lal_char(abc), expressionset<law_char(xyz), q>')

    In [3]: c.format('sname')
    Out[3]: 'letterset<char_letters(abc)>, expressionset<wordset<char_letters(xyz)>, q>'

    In [4]: c.format('text')
    Out[4]: '{abc} -> RatE[{xyz}* -> Q]'

    In [5]: c.format('utf8')
    Out[5]: '{abc} → RatE[{xyz}* → ℚ]'

When displaying values, the new format `utf8` improves the result.

    In [6]: e = c.expression('!(<x>a)*')

    In [7]: e.format('text')
    Out[8]: '(<x>a)*{c}'

    In [9]: e.format('utf8')
    Out[9]: '(⟨x⟩a)*ᶜ'

    In [10]: e.expansion().format('text')
    Out[10]: 'a.[(<x>a)*{c}] + b.[\\z{c}] + c.[\\z{c}]'

    In [11]: e.expansion().format('utf8')
    Out[11]: 'a⊙[(⟨x⟩a)*ᶜ] ⊕ b⊙[∅ᶜ] ⊕ c⊙[∅ᶜ]'

## 2015-12-08
### polynomial: conjunction
Conjunction of polynomials is now available in dyn. For polynomials of
expressionsets, compute the conjunction of the expressions. Otherwise, keep
the common labels and multiply their weights.

## 2015-12-05
### Syntactic sugar in expressions
One may now use UTF-8 when entering expressions.  The negation may also be
denoted by a prefix `!`, which binds weakly (less than concatenation).

    +--------+----------------+
    | Sugar  | Plain ASCII    |
    +========+================+
    | `∅`    | `\z`           |
    +--------+----------------+
    | `ε`    | `\e`           |
    +--------+----------------+
    | `⟨2⟩a` | `<2>a`         |
    +--------+----------------+
    | `a∗`   | `a*`           |
    +--------+----------------+
    | `!a`   | `a{c}`         |
    | `¬a`   |                |
    | `aᶜ`   |                |
    +--------+----------------+
    | `aᵗ`   | `a{T}`         |
    +--------+----------------+

## 2015-12-04
### weight_series: fix general case
This algorithm can now be applied to any automaton. In case of ℕmin weightset
the implementation does not change (shortest path). Otherwise, the result is
computed by applying proper on the `labels_are_one` version of the automaton.

### New trivial identities
Two new trivial identities have been added (at the "trivial" level):
neutrality of the universal language for conjunction (\z{c} & E => E, E &
\z{c} => E), and involutivity of complement on 𝔹 and 𝔽₂ (E{c}{c} => E).
It is not applied in the other case, since in ℤ (<2>a)*{c}{c} is actually
a*, not (<2>a)*.

## 2015-12-02
### left-mult and right-mult
The scalar product now also accept an argument to select the exact
algorithm: "standard", "general", "auto".  See 2015-11-13 for more details.

## 2015-12-02
### to-automaton: trim automata
Now `to_automaton` (or `expression.automaton` under Python) always produces
a trim automaton.

## 2015-11-29
### Improved display of letter classes
So far disjunction of letters were displayed as classes only if all
the letters had the same weight.  So `<1>a + <2>[^a]` in an automaton
resulted in an explicit list of letters.  This is especially
inappropriate for Levenshtein automata.  We now display `<1>a + <2>[^a]`.

## 2015-11-20
### weight_series: new algorithm
Compute the sum of all the weights of the accepted words (the sum of the
image of the behavior of the automaton). This algorithm can be applied to any
Nmin automaton, but requires acyclic automata for other weightsets.

## 2015-11-19
### trie and cotrie are enhanced
Both context.trie and context.cotrie accept `data`, `format` and `filename`
arguments.  In addition to `filename`, one can now pass directly a list of
words as a `data` string.  Use `format` to specify whether the lines are
 monomials or plain words --- for instance whether `<2>a` is the word `a`
with weight 2, or a four-letter word.

### Minimize: new algorithm: hopcroft
There is now a new implementation of the minimization, based on the
algorithm of Hopcroft.  It can only be used on Boolean automata with a free
labelset.  On many examples, this algorithm shows better performances than
"signature" but is still less efficient than "moore".

## 2015-11-13
### Automata: operations now work on all sorts of automata
Usual operations (`sum`, `multiply` --which includes the case for repeated
multiplication--, `star`) used to apply only to standard automata.  In
addition to `sum` (for standard automata), there was `union`, which applied
to any automata, but nothing for multiplication and star.

Now `sum`, `star` and `multiply` accept an `algo` argument:

"standard"
:   requires standard input automata, builds a standard automaton

"general"
:   applies to any kind of automaton, does not guarantee a standard
    automaton.  In the static API, might require a nullable labelset,
    in the dyn:: API and Python, might turn the labelset into a nullable
    one.

"auto" (default)
:   same as `"standard"` if input automata are standard, otherwise
    same as `"general"`.

In Python, the operators `+`, `*` and `**` use the `"auto"` strategy.

The `union` algorithms are removed, and in Python `|` no longer denotes it.
The left and right multiplication by a scalar were already implemented to
adapt standard or non standard automata.

## 2015-11-05
### Expansions: more operations
Usual operations (addition, multiplication, multiplication by a scalar) are
now available on expansions.

## 2015-10-15
### has_negative_cycles: new algorithm
Use automaton.has_negative_cycle to check whether an automaton has cycles
with negative weights.

### Minor bug fixes and improvements
- Spaces are now ignored in context names (e.g., ` lal < char (ab) >, b `).

- Expressions that have a single tape but several were expected are now
  properly rejected.

- Tuples now display parentheses only when needed.  For instance with
  weights in ℤ ⨉ ℤ, instead of `<(1, 2)>a`, we display `<1, 2>a`.


----------------------------------------------------------------------

# Vcsn 2.1 (2015-10-11)

About 10,000 hours (on the calendar, not of work!) after its first public
release, the Vcsn team is very happy to announce the release of Vcsn 2.1!

It is quite hard to cherry-pick a few new features that have been added in
Vcsn 2.1, as shown by the 4k+ lines of messages below since 2.0.  However,
here are a few headlines:

- Many pages of documentation and examples have been written (see
  http://vcsn.lrde.epita.fr/dload/2.1/notebooks).

- Now http://vcsn-sandbox.lrde.epita.fr/ provides a live demo.

- Transducers are much better supported, with improved syntax and several
  algorithms (e.g., letterize, synchronize, partial_identity, is_functional,
  etc.)

- Expressions now offer several sets of identities specifying how they
  should be normalized.  More generally, input/output of expressions have
  been improved to match most users' expectations.  New operators are
  accepted: `&` for conjunction, `:` for shuffle, `&:` for infiltration,
  `{c}` (postfix) for complement, and `<+` for deterministic choice.

- When entering an automaton (e.g., with `%%automaton` in IPython) user
  state names are preserved.

- Of course, many bugs were fixed, many algorithms were sped up, and
  internal details have been cleaned up.

- As Easter eggs, many features have also been added, but not advertised,
  until we are sure of how we want them to look like.

People who worked on this release:

- Akim Demaille
- Antoine Pietri
- Canh Luu
- Clément Démoulins
- Lucien Boillod
- Nicolas Barray
- Sébastien Piat
- Sylvain Lombardy
- Valentin Tolmer
- Yann Bourgeois--Copigny

People who have influenced this release:

- Alexandre Duret-Lutz
- Jacques Sakarovitch
- Luca Saiu


### Vcsn 2's repository has moved
To update your existing repository, run a command similar to:

    $ git remote set-url origin git@gitlab.lrde.epita.fr:vcsn/vcsn

## 2015-09-30
### shortest: applies to any automaton
The restriction to free-labelsets is lifted: shortest applies to automata
labeled with words, to transducers, etc.

## 2015-09-28
### project: new algorithm
Select a single tape from a multitape automaton (transducer).

## 2015-09-23
### expressions: spaces are now ignored
In an attempt to look like regexps, spaces were considered so far as
characters.

## 2015-09-17
### derived-term: deterministic automata
One can now require the construction of a deterministic automaton; there are
two new algorithms: `expansion,deterministic` and `derivation,deterministic`.

## 2015-09-15
### Named automata
It is (finally) possible to keep user state names thanks to
`name_automaton<Aut>`.  Now `dyn::read_automaton` and (in Python)
`vcsn.automaton` accept an additional `strip` argument (defaulting to true):
if set, the user names will be stripped once the automaton is loaded.  This
applies to all the supported formats: Daut, Dot, Efsm, and FAdo.

However using `%%automaton` names are kept by default.  Pass `-s`/`--strip`
to strip it.

## 2015-09-07
### Build fixes
Fixed (again) nasty shared-library issues at runtime on Ubuntu due to their
use of `-Wl,--as-needed`.  This affected Python only.

## 2015-08-31
### expression.info: more information
Now `atom` (the number of occurrences of letters) is also reported as
`width`, and the `depth` (also known as the `height`: the height of the
tree) is now included.

## 2015-08-26
### vcsn.automaton can guess the format
The vcsn.automaton function now accepts "auto" as format, which means "try
to guess from the content".

## 2015-08-25
### levenshtein: new algorithm
The `levenshtein` algorithm allows to build a transducer encoding the
Levenshtein distance between two alphabets. This transducer can then be
composed with two languages to obtain the edit-distance algorithm,
containing much information on the distance of the languages and their
words.

### partial_identity: new algorithm
The partial identity turns an automaton into a transducer realizing a partial
identity. I.e., for each accepted input, the output will be the same as the
input (plus the origin weight from the input automaton).

## 2015-08-18
### to_automaton: more algorithms
One can chose between both implementation of derived_term (stripped), using
"derivation" or "expansion".

## 2015-08-14
### to_expression: more heuristics
There are three new heuristics:

"delgado"
:   select a state whose removal would contribute a small
    expression (number of symbols, including `+`, etc.).

"delgado_label"
:   likewise, but count only the number of labels in the
    expression.

"best"
:   run all the heuristics, and return the shortest result.

The default algorithm remains "auto", which now denotes "best".

## 2015-08-03
### to_automaton: conversion from expression to automaton
Vcsn offers several means to build an automaton from an expression:
thompson, zpc, standard and derived-term.  The latter even builds a
decorated automaton, which, often, is not desired.

In C++ `dyn::to_automaton` and in Python `expression.automaton()` allow to
build a simple automaton from an expression.  It accepts an optional string
argument to select the conversion algorithm.  It defaults to "auto", which
currently means the stripped derived-automaton, but eventually, it will pick
either "standard" for basic expressions (as it's the fastest algorithm), or
"derived-term" for extended expressions (as "standard" does not support
these additional operators).

## 2015-06-18
### I/O in EFSM format are safer
We now correctly ensure the correspondence between our weightsets and
OpenFST's arc-type, and input and output.

As a consequence, we no longer support exchange of "traditional numerical"
weightsets (such as "Z", "R", etc.), since, as far as we know, they don't
feature a vis-à-vis in OpenFST.  It used to be accepted, but was
meaningless.

Boolean weights are mapped to "standard", OpenFST's tropical semiring.

When reading EFSM files, we still try to use the smallest corresponding
weightset.  For instance, if the arc type is "standard" and and the weights
are integral we use "zmin", otherwise, we use "rmin".  Errors on reading the
weight of final states have also been fixed.

## 2015-06-16
### automaton.expression, automaton.lift
They now accept an optional argument to specify the desired identities
for the expressions.

## 2015-06-15
### power, chain have been renamed
Before, "chain" denoted the repeated concatenation for automata and
expressions, and "power" meant repeated conjunction for automata and
expressions.  However, while `aut ** 2` meant `aut & aut` (conjunction),
`exp ** 2` meant `exp * exp` (concatenation).

Clearly, from the Python point of view, `**` is repeated `*`, which is what
most people would understand as "power".

So, to enforce consistency, and to avoid bad surprises, these functions were
renamed.

    +----------+---------------------+------------------------------------+
    | Python   | dyn::               | Comment                            |
    +==========+=====================+====================================+
    | `a * b`  | `multiply(a, b)`    | Multiplication for automata and    |
    |          |                     | expressions (concatenation),       |
    |          |                     | polynomials, weights, labels etc.  |
    +----------+---------------------+------------------------------------+
    | `a ** n` | `multiply(a, n)`    | Repeated multiplication.           |
    +----------+---------------------+------------------------------------+
    | `a & b`  | `conjunction(a, b)` | Conjunction for automata and       |
    |          |                     | expressions.                       |
    +----------+---------------------+------------------------------------+
    | `a & n`  | `conjunction(a, n)` | Repeated conjunction.              |
    +----------+---------------------+------------------------------------+

## 2015-06-11
### eliminate_state: fixes
Sometimes there was a mismatch between the state numbers as displayed, and
as expected by `eliminate_state`.  This is fixed.

The rendering was nonsensical when all the states were removed.  This is now
fixed.

Finally, passing -1 as argument, or no argument at all, delegates the choice
of the state to eliminate to a heuristic.

## 2015-06-04
### expressions: new identities sets
Rational expressions in Vcsn are "normalized" according to a set of
identities (such that `<0>E => \z` whatever the expression E is).

Vcsn now supports four different sets of identities:

"trivial"
:   a minimum set of transformations are applied.

"associative"
:   sum and product are made associative, so `a+(b+c)` and `(a+b)+c` are
    equal.

"linear"
:   sum is made commutative, and weights are factored, so `a+b+a` is equal
    to `a+b` in B, and to `<2>a+b` in Z.

"distributive" (or "series")
:   product and exterior/scalar products are distributed over sum, so
    `[ab]a` is equal to `aa+ba`, and `<2>[ab]` is equal to `<2>a+<2>b`.

Previously the default identities were "associative".  They are now
"linear", to match most users' expectations.

So, for instance we used to report:

    In [2]: c = vcsn.context('lal_char(a-z), z')

    In [3]: c.expression('r+[a-q]')
    Out[3]: r + [a-q]

    In [4]: c.expression('[a-q]+r+r')
    Out[4]: [^s-z] + r

we now report:

    In [3]: c.expression('r+[a-q]')
    Out[3]: [^s-z]

    In [4]: c.expression('[a-q]+r+r')
    Out[4]: [a-q] + <2>r

## 2015-05-29
### nmin: new weightset
The new tropical semiring ⟨ℕ, +, min⟩ has been introduced. Compared to zmin,
some optimizations can be done, for example in evaluation or in node
distance where the absence of negative weights allows to trim some branches.

## 2015-05-26
### cotrie: new algorithm
In additional to `trie`, which builds a deterministic tree-like automaton
(single initial state, multiple final states), vcsn now supports `cotrie`
which builds a "reversed trie": a codeterministic reversed tree-like
automaton (single final state, multiple initial states).

The main feature of `cotrie` is that its result is codeterministic, so it
only takes a determinization to minimize it.  It turns out that in Vcsn
determinization is more efficient than minimization:

    In [13]: %timeit c.trie('/usr/share/dict/words').minimize()
             1 loops, best of 3: 18.8 s per loop

    In [14]: %timeit c.cotrie('/usr/share/dict/words').determinize()
             1 loops, best of 3: 7.54 s per loop

These automata are isomorphic.

## 2015-05-22
### expressions: output may use label classes
Rational expressions have long supported label classes in input, e.g.,
[abc0-9].  Polynomials also support them in output.  However expressions
never used classes, which may seriously hinder their readability.  For
instance, to compute an expression describe all words on {a,..., z} except
'hehe', we had:

    In [2]: c = vcsn.context('lal_char(a-z), b')
            c.expression('(abcd){c}').derived_term().expression()
    Out[2]: \e+a(\e+b(\e+c))+(b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z+a(a+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z+b(a+b+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z+c(a+b+c+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z+d(a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z)))))(a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z)*

Now expressions also support letter classes in output:

    Out[2]: \e+a(\e+b(\e+c))+([^a]+a([^b]+b([^c]+c([^d]+d[^]))))[^]*

Classes are used only on ranges of at least four (unweighted) letters in
strictly increasing order:

    In [3]: c.expression('a+a+b+c+d+e+f+x+y+z+w')
    Out[3]: a+[a-fx-z]+w

Negated classes are issued only if the letters are in strictly increasing
alphabetical order, and are more than two thirds of the whole alphabet
(otherwise a regular class is preferred).

    In [4]: c.expression('[a-q]')
    Out[4]: [a-q]

    In [5]: c.expression('[a-q]+r')
    Out[5]: [^s-z]

    In [6]: c.expression('r+[a-q]')
    Out[6]: r + [a-q]

    In [7]: c.expression('[a-q]+r+r')
    Out[7]: [^s-z] + r

## 2015-05-21
### multiply: new name for 'concatenate'
In static, dyn:: and Python, `concatenate` is renamed `multiply`.

The "concatenation" of automata or rational expressions would never be
called the concatenation in the case of series or polynomials etc.  If we
were to support automata weighted by automata, the weightset product would
be this multiply.

## 2015-05-20
### trie: read directly from a file
Building a polynomial from a dictionary stored on disk, and then building
the trie is a waste of time.  It is now possible to build it directly from a
file.

    In [2]: t = vcsn.B.trie('/usr/share/dict/words')

    In [3]: t.info()
    Out[3]: {'is codeterministic': False,
             'is complete': True,
             'is deterministic': True,
             'is empty': False,
             'is eps-acyclic': True,
             'is normalized': False,
             'is proper': True,
             'is standard': True,
             'is trim': True,
             'is useless': False,
             'is valid': True,
             'number of accessible states': 792777,
             'number of coaccessible states': 792777,
             'number of codeterministic states': 792777,
             'number of deterministic states': 792777,
             'number of eps transitions': 0,
             'number of final states': 235886,
             'number of initial states': 1,
             'number of states': 792777,
             'number of transitions': 792776,
             'number of useful states': 792777,
             'type': 'mutable_automaton<letterset<char_letters()>, b>'}

## 2015-05-14
### trie: new algorithm
From a finite language/series represented as a polynomial (of words), build
an automaton that has the shape of a trie (a prefix tree).

    In [2]: series = '<2>\e+<3>a+<4>b+<5>abc+<6>abcd+<7>abdc'

    In [3]: p = vcsn.context('law_char, z').polynomial(series); p
    Out[3]: <2>\e + <3>a + <4>b + <5>abc + <6>abcd + <7>abdc

    In [4]: a = p.trie(); a
    Out[4]: mutable_automaton<letterset<char_letters(abcd)>, z>

    In [5]: a.shortest(100)
    Out[5]: <2>\e + <3>a + <4>b + <5>abc + <6>abcd + <7>abdc

## 2015-05-01
### delay_automaton: new automaton type
This algorithm transforms the automaton into an equivalent one, but where
each state has been split depending on the delay between the tape, i.e. the
difference of input length for each of the tapes.

### is-synchronized: new algorithm
This algorithm checks whether a transducer is synchronized, i.e. that the
input is read on every tape at the same rate for as long as possible.

### synchronize: new algorithm
This new algorithm allows to synchronize the tapes of a k-tape transducer,
i.e. "push" the letters towards the beginning of the transducer so that the
input is read along every tape at the same rate for as long as possible.

### has-bounded-lag: new algorithm
This algorithm checks whether a k-tape transducer has a bounded lag, i.e. if
there exists a constant D such that the length of the input for each tape
differs by at most D; or the output of the evaluation of a word through the
transducer differs in length by at most D.

## 2015-04-09
### shortest now subsumes enumerate
One can now specify both a maximum number of words, and a maximum size.  It
is also significantly faster.  The Python binding automaton.enumerate was
removed.

    In [4]: a = vcsn.Z.expression('[01]*1(<2>[01])*').standard()

    In [5]: a.shortest()
    Out[5]: 1

    In [6]: a.shortest(3)
    Out[6]: 1 + 01 + <2>10

    In [7]: a.shortest(len = 3)
    Out[7]: 1 + 01 + <2>10 + <3>11 + 001 + <2>010 + <3>011 + <4>100 + <5>101 + <6>110 + <7>111

    In [8]: a.shortest(len = 3, num = 5)
    Out[8]: 1 + 01 + <2>10 + <3>11 + 001

    In [9]: a.shortest(len = 30, num = 5)
    Out[9]: 1 + 01 + <2>10 + <3>11 + 001

## 2015-03-25
### product is renamed conjunction
After many hesitations, we finally decided to rename the synchronized
product from `product` to `conjunction`.  There are several reasons in favor
of this change.

First `product` is not adequate:

- we have several `products`: the Cauchy product (concatenation), the
  Hadamard product (synchronized product), the shuffle product, the
  infiltration product, and there are certainly more.

- `product` is already used in the case of words and rational expressions to
  denote the concatenation, so, to be consistent, `product` will denote the
  concatenation of automata too.

- `*` is naturally associated to `concatenation` for words and rational
  expressions, it was associated to `product` for automata.  It is more
  consistent to use `concatenate` in each case.

Second `conjunction` is a reasonable choice:

- `intersection` is acceptable for Boolean automata, but it is not
  satisfying in the case of weighted automata.

- `synchronized_product` is too long, likewise for `hadamard_product`, etc.

- `hadamard` is a proper name, which we try to avoid.

- the etymology of `conjunction` is a perfect match with the semantics of
  the operation.

- computer scientists are used to the correspondence between `conjunction`
  and `&`.

The preference for verbs as algorithm names leads naturally to `conjoin`.
However `conjunction` still seems more natural.

## 2015-03-21
### are-equivalent: now uses realtime
One may, finally, compare LAL, LAN, and LAW automata.

### join: improvements
The "join" of tuplesets now works properly, so one may, for instance,
add automata on A? x B and A x B? to get an automata on A? x B?.

In order to facilitate the experiments with `join`, it is now provided as an
algorithm.  In Python, it is also available as the infix `or` operator.

    In [2]: c1 = vcsn.context('lat<lal_char(a), lan_char(x)>, z'); c1
    Out[2]: {a} × ({x})? → ℤ

    In [3]: c2 = vcsn.context('lat<lan_char(b), lal_char(y)>, q'); c2
    Out[3]: ({b})? × {y} → ℚ

    In [4]: c1 | c2
    Out[4]: ({a,b})? × ({x,y})? → ℚ

## 2015-03-18
### infiltration: fix long standing bugs
Optimizations in the computation of the shuffle-product broke the
infiltration-product.  This is fixed.  However, the performances of
infiltration are then degraded (about 2.5x).  Conjunction (synchronized
product) and shuffle are unaffected.

Besides, the support for variadic infiltration products was naive, and
produced incorrect results.  This is fixed by simply repeating the binary
infiltration.

## 2015-03-17
### is_functional: new algorithm
Whether a transducer is functional, i.e., whether each input words maps to
(at most) a single word.

Actually, was implemented in September 2014, but was not registered here.

## 2015-03-16
### Python 3 is now required
We have tried hard to remain compatible with Python 2, but support for
Unicode is just too hard to get to work properly with both Python 2 and
Python 3.  Since there were constantly problems arising in one whose fixes
break the other, we decided to drop support for Python 2.  Given that all
major platforms ship Python 3, we don't expect this to be a real problem.

## 2015-03-15
### Repeated minimization
Now, minimization and cominimization return automata of the same type ---
cominimization used to return an automaton whose type reveals the double
transposition.  In actually, calling several times the minimization and/or
cominimization no longer generates decorators of decorators of etc.: the
result is always a single layer decorator.  Not only is this, in general,
the desired result, it's also more 'economic' as it uses fewer automaton
types (hence less runtime compilations).

## 2015-03-03
### realtime: new algorithm
Compute the letterized, proper equivalent of an automaton. The result
automaton will have only letter transitions (no words, no spontaneous
transitions).  It comes with the `is_realtime` algorithm, to check if an
automaton is realtime or not.

### is_letterized: new algorithm
This algorithm checks whether an automaton is letterized, i.e. whether each
transition's label is a single letter (in the sense of the labelset).

    In [2]: ctx = vcsn.context("law_char, b")

    In [3]: ctx.expression("abc").standard().is_letterized()
    Out[3]: False

    In [4]: ctx.expression("a*(b+c)").standard().is_letterized()
    Out[4]: True

## 2015-03-02
### zpc: new algorithm
This algorithm generates the ZPC automaton from an expression.  It has a
single initial state (whose final weight is the constant term of the
expression), a unique distinct final state (which cannot be reached from the
initial state via spontaneous transitions), and no cycles of spontaneous
transitions.

The `expression.zpc` function features an optional argument, which,
when set to "compact", enables a variant, more compact, construction.

    In [2]: vcsn.b.expression('ab').zpc()
        ╭───╮  \e   ╭───╮  a   ╭───╮  \e   ╭───╮  b   ╭───╮  \e   ╭───╮
    ──> │ 0 │ ────> │ 1 │ ───> │ 2 │ ────> │ 3 │ ───> │ 4 │ ────> │ 5 │ ──>
        ╰───╯       ╰───╯      ╰───╯       ╰───╯      ╰───╯       ╰───╯

    In [3]: vcsn.b.expression('ab').zpc('compact')
        ╭───╮  a   ╭───╮  \e   ╭───╮  b   ╭───╮
    ──> │ 0 │ ───> │ 1 │ ────> │ 2 │ ───> │ 3 │ ──>
        ╰───╯      ╰───╯       ╰───╯      ╰───╯

## 2015-02-23
### letterize: new algorithm
Create the equivalent automaton, but with only single-letter transitions.
Basically, do the conversion from law to lan. It also works recursively with
multitape transducers.

    In [2]: c = vcsn.context('lat<law_char, lal_char>, z')

    In [3]: a = c.expression("<2>'(abc,x)'").derived_term()

    In [4]: print(a.format('daut'))
    context = "lat<wordset<char_letters(abc)>, letterset<char_letters(x)>>, z"
    $ -> 0
    0 -> 1 <2>(abc,x)
    1 -> $

    In [5]: print(a.letterize().format('daut'))
    context = "lat<nullableset<letterset<char_letters(abc)>>, nullableset<letterset<char_letters(x)>>>, z"
    $ -> 0
    0 -> 2 <2>(a,x)
    1 -> $
    2 -> 3 (b,\e)
    3 -> 1 (c,\e)

## 2015-02-10
### Multitape expressions
When typing multitape transducers, it is now no longer necessary to explicit
the parenthesis inside (multi-)letters.

    In [1]: import vcsn

    In [2]: c = vcsn.context('lat<lan_char, lan_char>, b')

    In [3]: r = c.expression(r"'a,\e'+'(b,c)'+'d,f'")

### has_bounded_lag: new algorithm
This algorithm checks if a transducer has a bounded lag, i.e. if there is a
maximum difference of length between the input words and their corresponding
outputs.

    In [1]: import vcsn

    In [2]: c = vcsn.context('lat<lan_char, lan_char>, b')

    In [3]: c.expression(r"'a,\e'").standard().has_bounded_lag()
    Out[3]: True

    In [4]: c.expression(r"'a,\e'*").standard().has_bounded_lag()
    Out[4]: False

## 2014-11-18
### Flex
The Flex program (a scanner generator) should no longer be required for
builds from a tarball.

### blind -> focus
The `blind` algorithm (and the `blind_automaton` structure) has been renamed
`focus` (and `focus_automaton`) as it is much clearer on its purpose.

## 2014-11-14
### scc: new algorithm
Create an automaton whose states correspond with a strongly connected
component of the input automaton.

## 2014-11-13
### ratexp -> expression
The name "ratexp" is an unattractive jargon, and since Vcsn will not feature
other kinds of "expressions", it is hardly justified: it is the only
abbreviation we use (automaton, context, expansion, polynomial, etc.).

So _everywhere_ (static, dyn::, Python) "ratexp" was replaced by
"expression" (including "make_ratexpset" -> "make_expressionset", etc.).

## 2014-11-07
### thompson: the labelset no longer needs to feature a "one"
The Thompson automata count many spontaneous transitions, so require a
labelset which supports `\e` (e.g., `lan`, `law`).  This is annoying,
especially when teaching about Automata Theory: one does not want to dive
into the arcane details of contexts.

So now, if the context of the input expression does not support `\e`, the
Thompson automaton will be built with a generalized context which does
support it:

    In [1]: import vcsn

    In [2]: vcsn.context('lan_char, b').ratexp('a').thompson().context()
    Out[2]: lan<letterset<char_letters(a)>>, b

    In [3]: vcsn.context('law_char, b').ratexp('a').thompson().context()
    Out[3]: wordset<char_letters(a)>, b

    In [4]: vcsn.context('lal_char, b').ratexp('a').thompson().context()
    Out[4]: lan<letterset<char_letters(a)>>, b

## 2014-10-30
### filter: new algorithm
Hide states of another automaton, revealing only selected ones.  This does
not copy the original automaton.

## 2014-10-29
### New project name: Vcsn
The Vaucanson 2 project, which was funded by a (French) program (ANR), is
now "closed".  Of course, it will continue to exist, but now there will be
two Vaucansa!  One will be led by Sylvain Lombardy and Jacques Sakarovitch,
and the other by Alexandre Duret-Lutz and Akim Demaille from EPITA.

This file is about the latter, named "Vcsn".

## 2014-10-27
### WARNING: resyntaxed contexts
The syntax for contexts has changed: the separator between the labelset and
the weightset is now a comma (possibly with spaces) instead of an
underscore.

So for instance:

        lal_char(abc)_b
     -> lal_char(abc), b

        lat<lal_char(ab),lal_char(xy)>_lat<q,r>
     -> lat<lal_char(ab), lal_char(xy)>, lat<q, r>

        law_char(a-z)_ratexpset<law_char(A-Z)_b>
     -> law_char(a-z), ratexpset<law_char(A-Z), b>

This syntax is not (and has never been) the intended one, it should be
considered as the "internal" syntax.  However, since the intended syntax is
still not implemented, one, unfortunately, still has to deal with this inner
syntax.

## 2014-10-23
### proper: strip nullableset from labelsets
Our (current) implementation of `proper` is in-place, and as a consequence,
the result features the same context as the input automaton, with a nullable
labelset.

Now, `proper` copies the result in a context with a de-nullabled labelset.
So for instance:

    In [2]: vcsn.context('lan_char(ab)_b').ratexp('(ab)*').thompson().proper()
    Out[2]: mutable_automaton<lal_char(ab)_b>

There is no measurable performance regression.  However state numbers are
now unrelated to the input automaton.

## 2014-10-20
### Python: loading automata from files
The `vcsn.automaton` constructor now support a `filename` named argument
to load an automaton from a file.

    vcsn.automaton(filename = 'a.gv')
    vcsn.automaton(filename = 'a.efsm', format = 'efsm')

### minimize: default algorithm is "auto"
Now minimize and cominimize both support the "auto" algorithm, which is the
default value.  It uses the "signature" algorithm for the Boolean automata,
otherwise the "weighted" algorithm.

### proper: forward closure available in dyn:: and Python
It is now possible to require a forward elimination of spontaneous
transitions.  Calling `a.proper()` is equivalent to calling
`a.proper(prune = True, backward = True)`.

## 2014-10-07
### to-expression
The static and dyn:: algorithm that was named `aut_to_exp` is now named
`to_expression`.  In Python it is still `automaton.ratexp()`.

### costandard, is_costandard: new algorithms
Calling `aut.costandard()` is equivalent to calling
`aut.transpose().standard().transpose()`, and `aut.is_costandard()` is the
same with `aut.transpose().is_standard()`.

### normalize: new algorithm
Composes standard and costandard on an automaton.

## 2014-10-02
### Strip vs. transpose
Up to now, stripping a transposed automaton returned the non-transposed
automaton, without even stripping it.  This has finally been addressed, and
`a.determinize().transpose().strip()` is now strictly equivalent to
`a.determinize().strip().transpose()`.

### codeterminize, cominimize, is_codeterministic: new algorithms.
Calling `aut.cominimize()` is equivalent to calling
`aut.transpose().minimize().transpose()`, and likewise for `codeterminize`.

Available in dyn and Python.

## 2014-10-01
### split: now works on polynomials
In addition to splitting (aka, breaking) a ratexp, it is now possible to
split a polynomial.  This, for instance, provides another way to compute the
breaking derivation of a ratexp:

    In [1]: r = vcsn.context('lal_char_z').ratexp('a(b+a)+a(a+b)')

    In [2]: p = r.derivation('a'); p
    Out[2]: a+b + b+a

    In [3]: p.split()
    Out[3]: <2>a + <2>b

    In [4]: r.derivation('a', True)
    Out[4]: <2>a + <2>b

### derivation: fix a Python bug
`ratexp.derivation` features a `breaking` optional argument, which defaulted
to `True` instead of `False`.

### transpose: fix the number of final and initial states
Because `num_initials` and `num_finals` were not "transposed", on occasions
`automaton.is_standard` could crash (when it thinks there is one initial
state while there is none, but it still wants to check that its initial
weight is one).

## 2014-09-25
### Automaton conjunction
On occasions, in Python, expressions such as `(a & b)('a')` failed to behave
properly.  It does now, and does correspond to the evaluation of the word
"a" on the conjunction (synchronized product) of automata a and b.

### TikZ: state names
Conversion of automata into the TikZ format now includes the state "names"
(e.g., derived-term automata now show the states' rational expressions).

## 2014-09-21
### determinize: Empty-In, Empty-Out
Now when given an empty automaton (no states), determinize returns an empty
automaton, whereas it used to return an automaton with one state, initial.
This is more consistent (e.g., when given a deterministic automaton,
determinize now consistently returns the accessible part of its input).

## 2014-09-17
### is_cycle_ambiguous: new algorithm
Whether an automaton is cycle-ambiguous (or "exponentially ambiguous").

## 2014-09-09
### Build fixes
Portability issues with Mac OS X's own version of Flex are fixed.

Fixed runtime compilations failing to find Boost headers in some cases.

Fixed nasty shared-library issues at runtime on Ubuntu due to their use of
`-Wl,--as-needed`.

## 2014-09-04
### Fixes
I/O with OpenFST now properly supports initial weights and multiple initial
states.

Dot I/O of multiple-tape automata with initial weights is fixed.

Dot parse errors now provide locations.

The preconditions of reduce have been relaxed from labels-are-letters to
labelset-is-free.

## 2014-09-03
### determinize provides a better support for Z
The normalization of states now uses the GCD of the weights, which means
that now all the Z-automata that are determinizable can be determinized in
Z, without having to convert them as Q-automata.

### New weightset: qmp
Initial support for multiprecision rational numbers.

    >>> a = vcsn.context('lal_char(abc)_qmp').ratexp('<2/3>a').standard() & 70
    >>> a('a')
    1180591620717411303424/2503155504993241601315571986085849

Requires the GMP library.

## 2014-09-01
### compatibility with Boost 1.56
Some changes in Boost require adjustments.

## 2014-08-02
### ambiguous_word: new algorithm
Returns an ambiguous word of an automaton, or raises if the automaton is
unambiguous.

## 2014-07-30
### Several fixes
The reduce algorithm applied only to not-decorated automata.  Support for F2
was broken.

### Improvements
Error messages about failed algorithm instantiations are much clearer: they
display failed preconditions.

Blind is no longer limited to tape 0 or tape 1.


----------------------------------------------------------------------

# Vaucanson 2.0 (2014-07-25)

More than two years after the initial commit in our repository, the
Vaucanson team is happy to announce the first release of Vaucanson 2!

People who worked on this release:

- Akim Demaille
- Alexandre Duret-Lutz
- Alfred M. Szmidt
- Antoine Pietri
- Canh Luu
- Jacques Sakarovitch
- Luca Saiu
- Sylvain Lombardy
- Valentin Tolmer

## 2014-07-24
### "series" are now supported (static, dyn, Python)
Ratexpsets now take a constructor parameter specifying which set of
identities to enforce, currently with two possible values: "trivial" and
"series".  Old-style ratexps only support trivial identities, while series
also support sum commutativity and product distributivity over sum.
Heterogeneous operations involving both trivial ratexps and series ratexps
are possible.

In case of a ratexpset belonging to a context, the default value for the
optional parameter is always trivial.  The "seriesset" alias, which does not
accept an identity parameter, is also supported.

"Series" is technically a misnomer: the data structure is a better
approximation of the mathematical concept of series and in particular yields
a semiring structure, but actual series equality remains undecidable in the
general case.

Series currently depend on the commutativity of their weightset, and only
support a subset of the available operations.

    >>> vcsn.context('lal_char(a)_ratexpset<lal_char(x)_b>(series)') # series
    >>> vcsn.context('lal_char(a)_seriesset<lal_char(x)_b>')         # series
    >>> vcsn.context(ratexpset<lal_char(x)_b>_z')                    # trivial
    >>> vcsn.context('lal_char(a)_ratexpset<lal_char(x)_b>(trivial)')# trivial
    >>> e = vcsn.context('lal_char(a-c)_z').ratexp('b+a+b'); e
    b+a+b
    >>> s = vcsn.context('lal_char(a-c)_z').series('b+a+b'); s
    a+<2>b
    >>> (e+s).is_series()
    True

### has_twins_property: new algorithm
Computes whether an automaton has the twins property.

## 2014-07-16
### determinize supports weighted automata
The determinize algorithm (dyn and Python) now accepts an optional 'algo'
argument.

When 'algo' is:

"boolean"
:   the fast Boolean-only implementation is used.  It always terminates.

"weighted"
:    any weightset is supported.  On some inputs, it may not terminate.

"auto" (default value)
:    "boolean" if the automaton is Boolean, "weighted" otherwise.

## 2014-07-11
### are-equivalent is generalized
The 'are_equivalent' algorithm (a1.is_equivalent(a2) in Python) now supports
weights in fields (e.g., Q or R), but also on Z.  The labelset still must be
free.

## 2014-07-10
### reduce: new algorithm on automata
Implements the Schützenberger algorithm for reduction of representations for
any skew field.  As a special case, automata with weights in Z are also
supported.

## 2014-07-04
### determinization is more robust to large alphabets
In order to optimize (Boolean) determinization, 'determinize' no longer
accepts an optional 'complete' argument to require a deterministic complete
automaton.  One must now call 'complete()' afterwards.

The speed improvements are (erebus: OS X i7 2.9GHz 8GB, Clang 3.5 -O3 -DNDEBUG):

      (1)   (2)   (3)
     7.93s 7.80s 7.43s: a.determinize()      # a = ladybird(21)
     6.64s 6.45s 0.84s: a.determinize()      # a = lal(a-zA-Z0-9).ladybird(18)

where (1) is the "original" version, (2) is the version without the optional
completion of the determinized automaton (yes, it was set to False in the
bench of (1)), and (3) is the current version, which avoids considering
unused letters.

## 2014-07-01
### prefix, suffix, factor, subword: new algorithms
These four new functions (static, dyn and Python) take an automaton and
return another that accepts a superset of its language:

- suffix makes each accessible state final (with unit weight)

- prefix makes each coaccessible state initial

- factor makes each useful state initial and final

- subword applies the Magnus transform: for each non spontaneous
  transition (src, label, weight, dst), it adds a spontaneous transition
  (src, one, weight, dst).

## 2014-06-24
### minimize: new subset decorators, except for the "brzozowski" variant
Minimizing an automaton now yields a decorated automaton keeping track of
source state names.  The new "subset decorator" code is decoupled from
minimization and is intended to be used for other algorithms as well.

    >>> a = vcsn.context('lal_char(a-z)_b').ratexp('a+b*e+c+dc').standard(); a
    >>> a.minimize()

The minimize algorithm no longer recognizes the "brzozowski" variant at the
Static level, as it would require a very different, and likely
uninteresting, decorator; the user can still directly call
minimize_brzozowski at the Static level.  We still support "brzozowski" as a
variant at the Dyn and Python levels.

## 2014-06-23
### efsm: support for transducers
Exchange with OpenFST via efstcompile/efstdecompile now supports
transducers.

## 2014-06-15
### TAF-Kit is phased out
Existing commands for TAF-Kit are left and are occasionally useful.
However, today the last existing TAF-Kit test has been converted to Python:
TAF-Kit is no longer checked at all by the test suite.

### lift: now bound in Python
One can now lift automata and ratexps from Python.

## 2014-06-19
### Minimize: extend 'weighted' and 'signature' variants
Lift arbitrary restrictions on the labelset of the 'weighted' and
'signature' minimization variants.

    >>> ctx = vcsn.context('law_char(a-z)_b')
    >>> ctx.ratexp('ab+<3>cd+ac').standard().minimize('weighted')

## 2014-06-04
### More systematic use of decorators
In addition to the venerable `transpose_automaton`, several automaton
decorator types have been introduced (`blind_automaton`,
`determinized_automaton`, `pair_automaton`, `product_automaton`,
`ratexp_automaton`), and are now used in many algorithms, including:
`compose`, `determinize`, `product`, `shuffle`, `infiltration`,
`synchronizing_word`, etc.

Coupled with the fact that automata can now display state names (as
opposed to state numbers) in Dot output, one gets rich displays of
automata.  For instance:

    >>> ctx = vcsn.context('lal_char(ab)_b')
    >>> ctx.ratexp('aa+ab').derived_term().determinize()

now displays an automaton whose states are labeled as sets of ratexps:
"{aa+ab}", "{a, b}", and "{\e}".

Use `automaton.strip()` to remove state names.

## 2014-05-30
### dot2tex format
Automata now support for "dot2tex" format, meant to be used with the dot2tex
program.  It allows to combine TikZ's nice rendering of automata, LaTeX's
math mode to render state and transition labels, with dot automatic layout.

### IPython: an interactive display
Starting with IPython 2.0, running 'aut.display()' provides an interactive
display of an automaton, with means to select the display mode (e.g., "dot",
"dot2tex", etc.).

### shuffle: now available on ratexps
Available in static, dyn, and Python.

## 2014-05-28
### Optimization: composing two automata transposition yields the identity
Transposing an automata twice now yields the original automata, instead of
an automata wrapped by two decorator layers.

### is-isomorphic: extend to any context and any automata
Lift the previous limitation of is-isomorphic to deterministic lal automata.
The sequential case keeps its linear complexity, but the new generic code
has a worst-case complexity of O((n+1)!); however the common case is much
faster, as we heuristically classify states according to in- and
out-transitions, restricting brute-force search to states which are possible
candidates for isomorphism.

    >>> ctx = vcsn.context('lal_char(a-z)_b')
    >>> a = ctx.ratexp('ab+<3>ab+ab+ac').standard()
    >>> b = ctx.ratexp('ab+ac+<3>ab+ab').standard()
    >>> a.is_isomorphic(b)
    True
    >>> at = ctx.ratexp('abc').standard().transpose()
    >>> c = ctx.ratexp('cba').standard()
    >>> at.is_isomorphic(c)
    True

## 2014-05-22
### Automata are now handled via shared pointers
Types such as vcsn::mutable_automaton<Ctx> used to support the so called
"move semantics" only.  In particular, simple assignment between automata
was not possible.  This intentional limitation was introduced to avoid
accidental lose of performance by unexpected deep copies of automata, while
keeping partly "value semantics".

This, however, resulted in too many constraints, especially when one wants
to embed automata in other structures (which is for instance the case of
transpose_automaton which wraps an automaton).

To address these issues, types such as `vcsn::mutable_automaton<Ctx>` are
now `std::shared_ptr`.  This does change the programming style, both when
instantiating an automaton (typically now using `make_mutable_automaton`),
and when using it (with `->` instead of `.`):

Instead of:

    automaton_t aut{ctx};
    auto s1 = aut.new_state();
    aut.set_initial(s1);

write:

    auto aut = vcsn::make_mutable_automaton(ctx);
    // or: auto aut = vcsn::make_shared_ptr<automaton_t>(ctx);
    auto s1 = aut->new_state();
    aut->set_initial(s1);

## 2014-05-21
### Polynomials are now usable as weights
Polynomialsets are now usable as a generic weightset.  Polynomials are mostly
useful on law and ratexpset.

    >>> ctx = vcsn.context('lal_char(abc)_polynomialset<law_char(xyz)_z>')
    >>> ctx.ratexp('<x + xy + x + \e>a')
    <\e + <2>x + xy>a

## 2014-05-18
### ratexps: support for negated letter classes
We may now use '[^...]' to denote a letter other than the listed ones.  The
special case '[^]' denotes any character of the alphabet.

    >>> c = vcsn.context('lal_char(0-9)_b')
    >>> c.ratexp('0+[^0][^]*')
    0+(1+2+3+4+5+6+7+8+9)(0+1+2+3+4+5+6+7+8+9)*

### ratexps: invalid letter classes are rejected
Instead of being ignored, invalid intervals, or empty classes, are now
rejected.

    >>> c.ratexp('[9-0]')
    RuntimeError: invalid letter interval: 9-0
    >>> c.ratexp('[]')
    RuntimeError: invalid empty letter class

### ratexps: improved support for letter classes
Previously letter classes were supported only for context on top of a simple
alphabet (LAL, LAN and LAW).  Generators of more complex contexts such as
LAL x LAN are now supported:

    >>> c = vcsn.context('lat<lal_char(abc),lan_char(xyz)>_b')
    >>> c.ratexp("['(a,x)''(c,z)']")
    (a,x)+(c,z)

    >>> c.ratexp("[^'(a,x)''(c,z)']")
    (a,y)+(a,z)+(b,x)+(b,y)+(b,z)+(c,x)+(c,y)

    >>> c.ratexp("['(a,x)'-'(a,z)']")
    (a,x)+(a,y)+(a,z)

    >>> c.ratexp("[^]")
    (a,x)+(a,y)+(a,z)+(b,x)+(b,y)+(b,z)+(c,x)+(c,y)+(c,z)

## 2014-05-12
### compose: new algorithm (static, dyn, Python)
A new algorithm has been introduced to allow the composition of two
transducers.  It computes the accessible part of the transducer resulting
from the composition of the second tape of the first transducer with the
first tape of the second one.

    >>> c1 = vcsn.context('lat<lan_char(abc),lan_char(ijk)>_b')
    >>> c2 = vcsn.context('lat<lan_char(ijk),lan_char(xyz)>_b')
    >>> t1 = c1.ratexp("('(a,i)'+'(b,j)'+'(c,k)')*").thompson()
    >>> t2 = c2.ratexp("('(i,x)'+'(j,y)'+'(k,z)')*").standard()
    >>> t1.compose(t2).proper().shortest(8)
    (\e,\e) + (a,x) + (b,y) + (c,z) + (aa,xx) + (ab,xy) + (ac,xz) + (ba,yx)

## 2014-05-05
### insplit: new algorithm (static, dyn, Python)
It is now possible to do the in-splitting of an automaton, i.e. to get the
equivalent automaton such that each state have either only incoming
epsilon-transitions or no incoming epsilon-transitions.  This is the first
step in a product algorithm that supports epsilon-transitions.

## 2014-05-02
### Multiplication by a weight no longer requires a standard automaton
The right-multiplication by a weight uselessly required a standard
automaton; it now accepts any automaton.  The left-multiplication keeps its
specification for standard automata, but now supports non-standard automata,
in which case the weight is put on the initial arrows.

### Multiplication by 0 is fixed
The multiplication of an automaton by the null weight results in the "zero
automaton": a single state, initial, and no transitions.

## 2014-04-27
### Python syntactic sugar
Multiplication by a scalar on the left, and on the right, can be performed
with implicit conversion of the weights.  Instead of

    >>> z = vcsn.context('lal_char(ab)_z')
    >>> r = z.ratexp('[ab]*')
    >>> z.weight(2) * r * z.weight(3)

one can now write

    >>> 2 * r * 3

and similarly for automata.

Besides, repeated &-product can be denoted with the same symbol, &: 'a & 3'
denotes 'a & a & a'.

## 2014-04-22
### Vaucanson 2 has moved
Vaucanson 2 is now also hosted on gitlab.lrde.epita.fr.  It is also renamed
vaucanson.git, rather than vaucanson2.git.  To update your existing
repository, run a command similar to:

    $ git remote set-url origin git@gitlab.lrde.epita.fr:vcsn/vaucanson

or edit your .git/config file to update the URL.

You may also visit <http://gitlab.lrde.epita.fr/vcsn/vaucanson>.

### dyn::label is born
The family of dynamic object (which includes dyn::automaton, dyn::weight,
dyn::polynomial, dyn::ratexp, and others) now features a dyn::label.
Several algorithms dealing with labels used the "std::string" C++ type to
this end.  This was inadequate, and the corresponding signatures have been
cleaned bottom up.

For instance "derivation" used to have the following signatures

Static:

    template <typename RatExpSet>
    inline
    rat::ratexp_polynomial_t<RatExpSet>
    derivation(const RatExpSet& rs, const typename RatExpSet::value_t& e,
               const std::string& word, bool breaking = false)

Dyn:

    polynomial derivation(const ratexp& exp, const std::string& s,
                          bool breaking = false);

These signatures are now:

    template <typename RatExpSet>
    inline
    rat::ratexp_polynomial_t<RatExpSet>
    derivation(const RatExpSet& rs, const typename RatExpSet::value_t& e,
               const typename RatExpSet::labelset_t::word_t& word,
               bool breaking = false)


    polynomial derivation(const ratexp& exp, const label& l,
                          bool breaking = false);

At the Python level, derivation was adjusted so that one may still pass a
string, and see it upgraded, so both these calls work:

    >>> ctx = vcsn.context('lal_char(ab)_z')
    >>> r = ctx.ratexp('(<2>a)*')
    >>> r.derivation(ctx.word('aa'))
    <4>(<2>a)*
    >>> r.derivation('aa')
    <4>(<2>a)*

### Context extraction
It is now possible to obtain an automaton or a ratexp's context.  In dyn,
use "dyn::context_of(obj)", in Python use "obj.context()".

### Copy can now change the type of the automaton
vcsn::copy used to support only "homogeneous" duplications.  It also offers
access to the the origins (a map from resulting states to origin states).

### Variadic product of automata
The synchronized product of automata is now variadic: the product of
n-automata directly builds an automaton labeled with n-tuples of original
states.  The Python operator, &, is modified to pretend it is variadic
(rather than binary): it delays the computation until the result is needed.
The ".value()" method allows to force the evaluation.

    >>> ctx = vcsn.context('lal_char(ab)_z')
    >>> a1 = ctx.de_bruijn(5)
    >>> a = ctx.ratexp('a{5}').derived_term()
    >>> import timeit
    >>> timeit.timeit(lambda: (((a1&a1).value() & a1).value() & a).value(),
                      number=1000)
    1.9550061225891113
    >>> timeit.timeit(lambda: (a1 & a1 & a1 & a).value(), number=1000)
    0.5792131423950195

Be aware that if the result is not needed, then it is simply not computed at
all (hence, appears to be blazingly fast):

    >>> timeit.timeit(lambda: a1 & a1 & a1 & a, number=1000)
    0.0039250850677490234

### "Fine grain" runtime compilation works
So far dyn:: provided support for "context" runtime compilation, i.e., if a
context is unknown (e.g., "lat<lal_char(ab), lal_char(xy)>_b"), C++ code is
emitted, compiled, and loaded.  Now Vaucanson also supports per-algorithm
runtime compilation.

### Brzozowski minimization.
It is now possible to ask for the Brzozowski's minimization:

    >>> a = vcsn.context('lal_char(ab)_b').ratexp('a+b').standard()
    >>> a.minimize('moore').info()['number of states']
    2
    >>> a.minimize('signature').info()['number of states']
    2
    >>> a.minimize('brzozowski').info()['number of states']
    2

## 2014-04-11
### GCC is back in business
GCC compiles Vaucanson incorrectly (i.e., Vaucanson appears to behave
incorrectly, but it is actually the compiler that is incorrect).  For this
reason, some features were disabled with such a compiler, and recently GCC
support was completely dropped.

Efforts were put in finding reasonable workarounds for these bugs, and now
GCC is, again, supported.  Parts that used to be disabled are now supported.

For more information on this bug, see the following problem report
<http://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253>.

## 2014-04-10
### Vaucanson 1 has moved
Vaucanson 1 is now hosted on gitlab.lrde.epita.fr.  It is also renamed
vaucanson1.git, rather than just vaucanson.git.  To update your existing
repository, run a command similar to:

    $ git remote set-url origin git@gitlab.lrde.epita.fr:vcsn/vaucanson1

or edit your .git/config file to update the URL.

You may also visit <http://gitlab.lrde.epita.fr/vcsn/vaucanson1>.

## 2014-04-05
### conjunctions of ratexps
The `intersection` operation on ratexps is renamed as `conjunction`.

## 2014-03-28
### Improve treatment of nullable labelsets
As introduced earlier, nullablesets are now written `lan<...>`, for instance
`lan<lal_char(ab)>_b`.  The former syntax, e.g., `lan_char(ab)_b`, is kept
for backward compatibility and ease of use.

Up to now, `lan<>` would only work on lal and labelsets with already a `one`
provided, making the improvement quite useless.  Now `lan<...>` will
manufacture a `one` for the labelsets that don't have one, so it can be
wrapped around anything.

In addition to that, simplifications are applied; for instance
`lan<lan<lan_char(ab)>>_b` actually builds `lan<lal_char(ab)>_b`.
Similarly, `lan<lat<lan_char(ab), law_char(ab), ratexpset<lal_char(ab)_b>>>_b`
generates `lat<lan<lal_char(ab)>, law_char(ab), ratexpset<lal_char(ab)_b>>_b`,
since the tuple wrapped in the outer lan already has a `one`, given that all
of its components have one.

## 2014-03-26
### concatenation and conjunction of ratexps accept more heterogeneous arguments
Conversions of both labels and weights are performed if needed.

    >>> a = vcsn.context('ratexpset<lal_char(xy)_b>_z').ratexp("<2>'x*'")
    >>> b = vcsn.context('lal_char(b)_q')              .ratexp('<1/3>b')
    >>> a * b
    <2>x*<1/3>b
    >>> (a*b).info()['type']
    'ratexpset<ratexpset<lal_char(bxy)_b>_q>'

    >>> ab = vcsn.context('lal_char(ab)_z').ratexp('(a+b)*')
    >>> bc = vcsn.context('lal_char(bc)_b').ratexp('(b+c)*')
    >>> ab & bc
    (a+b)*&(b+c)*
    >>> (ab & bc).info()['type']
    'ratexpset<lal_char(abc)_z>'

## 2014-03-09
### left-mult and right-mult are bound in Python
Both work on automata and ratexps.  Left multiplication now has its
arguments in a more natural order in the C++ API: (weight, automaton),
instead of the converse previously.  TAF-Kit still has it the old way.  The
Python operator * (left-associative) is overloaded to provide syntactic
sugar.

    >>> z = vcsn.context('lal_char(abc)_z')
    >>> a = z.weight("12") * z.ratexp('ab').standard() * z.weight("23")
    >>> a.ratexp()
    (<12>ab)<23>

    >>> z.weight("12") * z.ratexp('ab') * z.weight("23")
    <12>(ab)<23>

## 2014-03-08
### sum of rational expressions accepts more heterogeneous arguments
Conversions of both labels and weights are performed if needed.

    >>> a = vcsn.context('ratexpset<lal_char(xy)_b>_z').ratexp("<2>'x*'")
    >>> b = vcsn.context('lal_char(b)_q')              .ratexp('<1/3>b')
    >>> a + b
    <2>x*+<1/3>b

### concatenation accepts more heterogeneous arguments
As for products and union, it is now possible to compute the concatenation
of automata with different types:

    >>> z = vcsn.context('lal_char(a)_z').ratexp('<2>a')  .derived_term()
    >>> q = vcsn.context('lal_char(b)_q').ratexp('<1/3>b').derived_term()
    >>> r = vcsn.context('lal_char(c)_r').ratexp('<.4>c') .derived_term()
    >>> (z*q*r).ratexp()
    <2>a<0.333333>b<0.4>c

## 2014-03-05
### Bug: standardization of automata
The standardization of an automaton no longer leaves former initial states
that became non-accessible.

### Bug: right-mult on automata
Its behavior was completely wrong.

## 2014-02-21
### parse and display letter classes in transitions
It is now possible to directly write labels of transitions with letter
classes.  For instance '[a-ky]' denotes every letter between a and k in the
alphabet, or y.

Transition labels with equal weights are displayed this way.  For instance
'g, a, b, d, c, f' becomes [a-dfg] and '<2>a, <2>b, <2>c" is displayed
'<2>[abc]', but 'a, b' stays in this form.

## 2014-02-20
### union accepts more heterogeneous arguments
As for products (Hadamard, shuffle, infiltration), it is now possible to
compute the union of automata with different types:

    >>> z = vcsn.context('lal_char(a)_z').ratexp('<2>a')  .derived_term()
    >>> q = vcsn.context('lal_char(b)_q').ratexp('<1/3>b').derived_term()
    >>> r = vcsn.context('lal_char(c)_r').ratexp('<.4>c') .derived_term()
    >>> (z|q|r).ratexp()
    <2>a+<0.333333>b+<0.4>c

### automaton product optimization.
An optimization enabled by state renumbering.

Score changes on Luca's workstation (before/after):

    4.60s  2.87s: a.product(a)         # a = std([a-e]?{50})
    2.34s  2.37s: a.shuffle(a)         # a = std([a-e]?{50})
    4.01s  2.58s: a.infiltration(a)    # a = std([a-e]?{30})
    4.17s  2.96s: a**12                # a = std([a-e]*b(<2>[a-e])*)

## 2014-02-18
### state renumbering is not automatic any longer
In view of several forthcoming in-place algorithms on automata including
edit, we changed the automaton output code not to automatically renumber
states at the static, dyn and Python levels.  A new "sort" algorithm is
available to renumber states (breadth-first and then by outgoing transition
label) and reorder transitions (by label) in a predictable way, when
explicitly requested.  TAF-Kit output is automatically sorted.

Having a predictable numbering will enable future optimizations on automaton
product and other algorithms accessing all the transition from a given state
by label.

## 2014-02-14
### double_ring: New Python binding
Returns a double ring automaton.

## 2014-02-11
### ratexpset can be used for LabelSet
Contexts such as `ratexpset<lal_char(ab)_b>_b` are now valid.  The
eliminate_state algorithm works properly on automata of such type.

## 2014-02-10
### ratexp.info: New Python binding
Like automaton.info, returns a dictionary of properties of the ratexp.

## 2014-02-07
### are-isomorphic/is-isomorphic: new algorithm (static, dyn, TAF-Kit, Python)
Given two automata, check whether they are isomorphic to one another.
Currently implemented in the deterministic case only, for lal contexts.

    >>> ctx = vcsn.context('lal_char(ab)_z')
    >>> ctx.ratexp('a+b*').standard().is_isomorphic(ctx.ratexp('b*+a').standard())
    True

### weighted minimization (static, dyn, tafkit, Python)
A third implementation of the minimization algorithm named "weighted" is now
available, supporting any lal context.

The new variant is a more widely applicable generalization of the
"signature" implementation.  The new variant is the default for non-boolean
weightsets; it requires a trim automaton but does not rely on determinism or
other properties.  Preliminary measures show performance to be close to
"signature", or even clearly superior in the case of sparse automata such as
dictionaries.

----------------------------------------------------------------------

# Vaucanson 2b.3 (2014-02-03)
Release of our fourth beta, vaucanson-2b.3.  Available on MacPorts as
"vaucanson".

See <https://www.lrde.epita.fr/wiki/Vaucanson/Vaucanson2b3>.

### A Virtual Machine for easy experiments
Installing Vaucanson 2 on some platforms can be tedious.  Clément Démoulins
contributed a Virtual Machine that makes it easy to experiment with Vaucanson
2 without having to compile it.  He also contributed a Vagrantfile to make
it even easier to deploy.

To install a Vaucanson virtual machine, please follow this procedure:

    1. Install VirtualBox
       From your distro, or from https://www.virtualbox.org/wiki/Downloads.

    2. Install Vagrant
       From your distro, or from http://www.vagrantup.com/downloads.html

    3. Download this Vagrantfile and save it somewhere.
       https://www.lrde.epita.fr/dload/vaucanson/2.0/Vagrantfile

       For instance
       $ mkdir ~/src/vcsn2
       $ cd ~/src/vcsn2
       $ wget https://www.lrde.epita.fr/dload/vaucanson/2.0/Vagrantfile

    4. Run Vagrant (first time will be slow: let it download the VM)

       $ cd ~/src/vcsn2
       $ vagrant up

       Vaucanson is running!

    5. Open http://localhost:8888 in your favorite browser.

    6. Experiment!  (Hit Shift-Enter to evaluate):
       import vcsn
       vcsn.context('lal_char(abc)_z').ratexp('(<2>a+<3>b)*').derived_term()

    7. Turn your VM off when you are done
       $ vagrant halt

## 2014-01-29
### ratexp difference: new algorithm (static, dyn, Python)
The "difference" algorithms generates a ratexp that accepts words of the
left-hand side that are not accepted by the right-hand side ratexp.  Also
bound as the "%" operator in Python.

    >>> ctx = vcsn.context('lal_char(abc)_b')
    >>> l = ctx.ratexp('[abc]*')
    >>> r = ctx.ratexp('[ab]*')
    >>> l.difference(r)
    (a+b+c)*&(a+b+c)*
    >>> l % r
    (a+b+c)*&(a+b+c)*

## 2014-01-27
### ratexp intersection: new algorithm (static, dyn, Python)
The "intersection" algorithm computes a ratexp that denotes the Hadamard
product of two rational expressions.  Also bound as the "&" operator in
Python.

    >>> ctx = vcsn.context('lal_char(abc)_b')
    >>> r = ctx.ratexp('[abc]*')
    >>> r.intersection(r)
    (a+b+c)*&(a+b+c)*
    >>> r & r
    (a+b+c)*&(a+b+c)*

### ratexp concatenation: new algorithm (static, dyn, Python)
The "concatenate" algorithm computes a ratexp that denotes the concatenation
of two rational expressions.  Also bound as the "*" operator in Python.

    >>> ctx = vcsn.context('lal_char(abc)_b')
    >>> r = ctx.ratexp('[abc]*')
    >>> r.concatenate(r)
    (a+b+c)*(a+b+c)*
    >>> r * r
    (a+b+c)*(a+b+c)*

### Python API: Operators overloading on automata
The Python API now overloads the following operators for automata:

- `+`, sum of two automata
- `&`, product of two automata
- `~`, complement of an automaton
- `*`, concatenation of two automata
- `%`, difference between two automata
- `|`, union of two automata
- `**`, power of an automaton

## 2014-01-24
### ratexp sum: new algorithm (static, dyn, Python)
Compute the sum of two rational expressions.  Also bound as the `+` operator
in Python.

    >>> ctx = vcsn.context('lal_char(abc)_b')
    >>> r = ctx.ratexp('[abc]*')
    >>> r.sum(r)
    (a+b+c)*+(a+b+c)*
    >>> r + r
    (a+b+c)*+(a+b+c)*

### proper: an optional argument to avoid state deletion
The proper algorithm eliminates the states that become inaccessible in the
course of spontaneous-transition elimination.  This can be disabled by
passing "false" as additional argument to proper (static, dyn, Python).

## 2014-01-17
### star_height: new algorithm (static, dynamic, Python)
Computes the star-height of an expression.

    >>> vcsn.context('lal_char(ab)_b').ratexp('(a***+a**+a*)*').star_height()
    4

### Bison is no longer needed
To install Vaucanson from a tarball, Bison is no longer needed.  Of course,
modifying one of grammar files will fail, unless Bison 3.0 is installed.

## 2014-01-13
### Letter classes in context specifications
It is now possible to use ranges to define alphabets.  For instance in
Python,

    vcsn.context('lal_char(a-zA-Z0-9_)_b')

builds a context whose alphabet covers letters, digits, and underscore.

----------------------------------------------------------------------

# Vaucanson 2b.2 (2014-01-10)
Release of our third beta, vaucanson-2b.2.  Available on MacPorts as
"vaucanson".

### Letter classes
Letter classes are available in ratexps: `[a-d0-9_]` is expanded into
`(a+b+c+d+0+1+2+3+4+5+6+7+8+9+_)`.  The negation, `[^...]`, is not supported.

## 2014-01-02
### The '.' operator is no longer printed
The pretty-printing of (non-LAW) ratexps is simplified.

Before:

    $ vcsn ladybird 2 | vcsn determinize | vcsn aut-to-exp
    \e+a.(b+a.a+c.(a+c)*.b)*.(a+c.(a+c)*)
    $ vcsn derived-term -Ee 'a:b:c' | vcsn aut-to-exp
    (a.b+b.a).c+(a.c+c.a).b+(b.c+c.b).a

After:

    $ vcsn ladybird 2 | vcsn determinize | vcsn aut-to-exp
    \e+a(b+aa+c(a+c)*b)*(a+c(a+c)*)
    $ vcsn derived-term -Ee 'a:b:c' | vcsn aut-to-exp
    (ab+ba)c+(ac+ca)b+(bc+cb)a

### Shortlex order is now used for ratexps
This changes the pretty-printing of polynomials of ratexps, for instance the
result of derivations.

Before:

    $ vcsn derivation -e '(a*+b*)a(a*+b*)' aa
    a*+b* + a*.a.(a*+b*) + a*

After:

    $ vcsn derivation -e '(a*+b*)a(a*+b*)' aa
    a* + a*+b* + a*a(a*+b*)

### ratexp implementation overhaul
Although internal, this change is documented as it deeply changes the way
ratexps are handled in C++ code.

The abstract-syntax tree of the rational expressions now matches the usual
(abstract) grammar:

    E ::= \z | \e | a | E+F | E.F | E* | kE | Ek       k is a weight

In other words, there are now 'left-weight' and 'right-weight' nodes that
exist, whereas before, the six first cases carried left and right weights.
Trivial identities are enforced, and, for instance, no tree for 'a k' exist:
it is converted to 'k a'.

## 2013-12-19
Many of these listed features were actually developed over the last month.

### products accept more heterogeneous arguments
It is already possible to compute the (regular, infiltration and shuffle)
products of automata with different types of "basic" weightsets (e.g. B, Z,
Q, R).

It is now also possible when weights are ratexps.  For instance the product
of an automaton with weights in Q and an automaton with weights in RatE
yields an automaton with weights in Q-RatE.

### New ratexp operators: `&` (intersection) and `:` (shuffle)
The operator `&` denotes the intersection in the case of Boolean weights, or
more generally, the Hadamard product.  Only "derived_term" can compute an
automaton from it, in which case:

    derived_term(E & F) = product(derived_term(E), derived_term(F))

The operator `:` denotes the shuffle product, aka interleave.  For instance
`a:b:c` denotes the language of the permutations of "abc".  Only
"derived_term" can compute an automaton from it, in which case:

    derived_term(E : F) = shuffle(derived_term(E), derived_term(F))

### minimization is much faster
There are now two different implementations of the minimization, namely
"moore" and "signature".  Both require a trim automaton as input, and
"signature" accepts non-deterministic automata.

The "moore" minimization is currently the fastest.

### context run-time compilation
When dyn::make_context is presented with an unknown but valid context, it is
compiled and loaded dynamically.  For instance:

    $ vcsn cat -C 'lao_r' -W -e 3.14
    # Wait for the context to be compiled...
    3.14

The compiled context is currently left in /tmp for forthcoming runs.

    $ ls /tmp/lao*
    /tmp/lao_r.cc   /tmp/lao_r.o   /tmp/lao_r.so

### Python binding
It is now possible to use Vaucanson from Python.  The Python binding is on
top of the dyn API, and inherits all its features.

This Python API is object-oriented, contrary to dyn which is a list of types
(context, ratexp, automaton, etc.) and functions (derived_term, determinize,
etc.):

    +---------------------------------------+-----------------------------------------+
    | dyn:: functions                       | Python methods                          |
    +=======================================+=========================================+
    | - derived_term(ratexp) -> automaton   | - ratexp.derived_term() -> automaton    |
    | - determinize(automaton) -> automaton | - automaton.determinize() -> automaton  |
    | - etc.                                | - etc.                                  |
    +---------------------------------------+-----------------------------------------+

Documentation is forthcoming, but for instance:

    $ python
    Python 2.7.6 (default, Nov 12 2013, 13:26:39)
    [GCC 4.2.1 Compatible Apple Clang 4.1 ((tags/Apple/clang-421.11.66))] on darwin
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import vcsn
    >>> b = vcsn.context('lal_char(abc)_b')
    >>> b
    lal_char(abc)_b
    >>> r1 = b.ratexp('(a+b)*')
    >>> r1
    (a+b)*
    >>> a1 = r1.derived_term()
    >>> a2 = b.ratexp('(b+c)*').standard()
    >>> a1.product(a2).ratexp()
    \e+b.b*
    >>>

## 2013-12-03
### products accept heterogeneous arguments
It is now possible to compute the (regular, infiltration and shuffle)
products of a (B, Z, Q, R) automaton with a (B, Z, Q, R) automaton.

    $ vcsn derived-term -C 'lal_char(ab)_q' -e '(<1/2>a+<1/3>b)*' > q.gv
    $ vcsn derived-term -C 'lal_char(ab)_z' -e '(<2>a+<3>b)*'     > z.gv
    $ vcsn derived-term -C 'lal_char(ab)_r' -e '(<.2>a+<.3>b)*'   > r.gv

    $ vcsn product -f z.gv q.gv | vcsn aut-to-exp
    (a+b)*
    $ vcsn product -f q.gv z.gv | vcsn aut-to-exp
    (a+b)*
    $ vcsn product -f q.gv z.gv q.gv | vcsn aut-to-exp
    (<1/2>a+<1/3>b)*
    $ vcsn product -f q.gv z.gv z.gv | vcsn aut-to-exp
    (<2>a+<3>b)*
    $ vcsn product -f q.gv r.gv | vcsn aut-to-exp
    (<0.1>a+<0.1>b)*
    $ vcsn product -f z.gv r.gv | vcsn aut-to-exp
    (<0.4>a+<0.9>b)*
    $ vcsn product -f z.gv r.gv q.gv | vcsn aut-to-exp
    (<0.2>a+<0.3>b)*

## 2013-12-02
### TikZ output now uses standalone
The TikZ output can now both be compiled as a standalone document, and be
used inlined (as LaTeX source) in another document.  Read the document of
the "standalone" package (e.g., run 'texdoc standalone').

### Data library path
TAF-Kit is now able to find installed files.  Control the search path via
the environment variable VCSN_DATA_PATH (a ":"-separated list of
directories).

    $ vcsn aut-to-exp -f lal_char_z/c1.gv
    (a+b)*.b.(<2>a+<2>b)*

### dot format parsing is much faster
We now require Bison 3.0 to build Vaucanson.  In exchange, we get is very
significant speed-up.  On erebus (Mac OS X i7 2.9GHz 8GB, GCC 4.8 -O3)
reading standard('a?{500}') goes from 30s to 1.2s.

## 2013-11-25
### minimize: new algorithm (static, dynamic, TAF-Kit)
Given a deterministic Boolean lal automaton, compute its minimal equivalent
automaton using Moore's algorithm.

Example:

    $ vcsn standard -e '(a+b+c+d){100}' -o 100.gv
    $ vcsn cat -f 100.gv -O info | grep 'number of states'
    number of states: 401
    $ vcsn minimize -f 100.gv -o 100min.gv
    $ vcsn cat -f 100min.gv -O info | grep 'number of states'
    number of states: 101
    $ vcsn are-equivalent -f 100.gv 100min.gv
    true

### fraction parsing bug fixes
Negative denominators used to be silently mangled, and zero denominators were
accepted without failing.

## 2013-11-22
### split: new algorithm (static, dynamic, TAF-Kit)
Implements the breaking of a ratexp into a polynomial of ratexps, as
required by breaking derivation, and broken derived terms.  Not named
"break", as this is a C++ keyword.

## 2013-11-20
### products accept heterogeneous arguments
It is now possible to compute the (regular, infiltration and shuffle)
products of a B-automaton with any other kind of automaton.

    $ vcsn standard -C 'lal_char(ab)_b' -e 'a' -o a.gv
    $ vcsn standard -C 'lal_char(ab)_ratexpset<lal_char(uv)_b>' \
                    -e '(<u>a+<v>b)*' -o ab.gv
    $ vcsn product -f ab.gv a.gv | vcsn shortest -f - 4
    <u>a
    $ vcsn shuffle -f ab.gv a.gv | vcsn shortest -f - 4
    a + <u+u>aa + <v>ab + <v>ba
    $ vcsn infiltration -f ab.gv a.gv | vcsn shortest -f - 4
    <u+\e>a + <u.u+u+u>aa + <u.v+v>ab + <v.u+v>ba

## 2013-11-19
### shortest: accepts the number of words as argument (defaults to 1)

    $ vcsn shortest -O text -Ee '(a+b)*' 2
    \e + a
    $ vcsn enumerate -O text -Ee '(a+b)*' 2
    \e + a + b + aa + ab + ba + bb

### difference: new algorithm (static, dynamic, TAF-Kit)
Computes the difference between an automaton and a B-automaton (on LAL
only):

    $ vcsn derived-term -e '(?@lal_char(ab)_z)(<2>a+<3>b)*' -o lhs.gv
    $ vcsn derived-term -e '(a+b)*b(a+b)*' -o rhs.gv
    $ vcsn difference -f lhs.gv rhs.gv | vcsn aut-to-exp
    (<2>a)*
    $ vcsn difference -f rhs.gv rhs.gv | vcsn aut-to-exp
    \z

### product accepts heterogeneous arguments
It is now possible to compute the product of (for instance) a Z-automaton
with a B-automaton.

    $ vcsn standard -C 'lal_char(abc)_z' -e '(<2>a+<3>b+<5>c)*' -o 1.gv
    $ vcsn standard -C 'lal_char(b)_b' -e 'b*' -o 2.gv
    $ vcsn product -f 1.gv 2.gv | vcsn aut-to-exp
    \e+<3>b.(<3>b)*
    $ vcsn product -f 2.gv 1.gv | vcsn aut-to-exp
    \e+<3>b.(<3>b)*

----------------------------------------------------------------------

# Vaucanson 2b.1 (2013-11-13)
Release of our second beta, vaucanson-2b.1.  Available on MacPorts as
"vaucanson".

## 2013-11-07
### TAF-Kit: infiltration, product, shuffle: accept multiple arguments
You may pass several (one or more) arguments to vcsn product, infiltration
and shuffle.

### infiltration, product, shuffle: accept non commutative semirings
Beware that the (well defined) behavior of the resulting automata is no
longer what one might expect.

    $ vcsn standard -C 'lal_char(ab)_ratexpset<lal_char(uv)_b>' \
                    -e '<u>a<v>b' -o uv.gv
    $ vcsn standard -C 'lal_char(ab)_ratexpset<lal_char(xy)_b>' \
                    -e '<x>a<y>b' -o xy.gv
    $ vcsn product -f uv.gv xy.gv | vcsn enumerate -f - 4
    <u.x.v.y>ab
    $ vcsn shuffle -f uv.gv xy.gv | vcsn enumerate -f - 4
    <u.x.v.y+u.x.y.v+x.u.v.y+x.u.y.v>aabb
    <u.v.x.y+x.y.u.v>abab
    $ vcsn infiltration -f uv.gv xy.gv | vcsn enumerate -f - 4
    <u.x.v.y>ab
    <u.x.v.y+x.u.v.y>aab
    <u.x.v.y+u.x.y.v>abb
    <u.x.v.y+u.x.y.v+x.u.v.y+x.u.y.v>aabb
    <u.v.x.y+x.y.u.v>abab

### Input/Output fixes
Support for EFSM in input/output is improved to support weights.  Output was
significantly sped up.

Before:

    0.64s: standard -Ee 'a?{500}' -O dot  >a500.gv
    4.47s: standard -Ee 'a?{500}' -O efsm >a500.efsm
    4.04s: standard -Ee 'a?{500}' -O fado >a500.fado
    4.83s: standard -Ee 'a?{500}' -O grail>a500.grail
    1.03s: standard -Ee 'a?{500}' -O tikz >a500.tikz

After:

    0.76s: standard -Ee 'a?{500}' -O dot  >a500.gv
    0.37s: standard -Ee 'a?{500}' -O efsm >a500.efsm
    0.35s: standard -Ee 'a?{500}' -O fado >a500.fado
    0.35s: standard -Ee 'a?{500}' -O grail>a500.grail
    0.68s: standard -Ee 'a?{500}' -O tikz >a500.tikz

## 2013-11-04
### boolean weight parsing bug fix
The weight parser used to ignore all characters after the first one, so that
for example "1-q" was considered valid and equivalent to "1".

## 2013-10-31
### product and infiltration-product are much faster
On erebus (MacBook Pro i7 2.9GHz 8GB, GCC 4.8 -O3), with "vcsn standard -E
-e 'a?70' -o a70.gv":

Before:

       13.01s: product -q -f a70.gv a70.gv
        0.23s: shuffle -q -f a70.gv a70.gv
       16.64s: infiltration -q -f a70.gv a70.gv
After:

        0.77s: product -q -f a70.gv a70.gv
        0.19s: shuffle -q -f a70.gv a70.gv
        0.87s: infiltration -q -f a70.gv a70.gv

## 2013-10-30
### TAF-Kit: option name changes
The former options -W and -L (to specify the weightset and labelset) are
removed, definitively replaced by -C, to specify the context.

In addition to -A and -E (input is an automaton/ratexp), option -W now
replaces option -w (input is a weight), and option -P denotes "polynomials".
Note that "polynomials" are actually linear combinations of labels, weighted
by weights, so for instance "LAL" contexts accept only single-letter labels,
used LAW to accept words.

    $ vcsn cat -C 'law_char(ab)_z' -P -e 'a+ab + <-1>a+ab+<10>bb'
    <2>ab + <10>bb

### internal overhaul
Better names were chosen for the various details of the dyn:: value support:

               abstract_automaton -> automaton_base

                 abstract_context -> context_base

              abstract_polynomial -> polynomial_base
     concrete_abstract_polynomial -> polynomial_wrapper

                  abstract_ratexp -> ratexp_base
         concrete_abstract_ratexp -> ratexp_wrapper

               abstract_ratexpset -> ratexpset_base
      concrete_abstract_ratexpset -> ratexpset_wrapper

                  abstract_weight -> weight_base
         concrete_abstract_weight -> weight_wrapper

Note that `_base` is slightly misleading, as it actually applies to the
wrappers, and not to the static objects.  For instance, weightset_wrapper
derives from weightset_base, but `b`, `z`, and the other (static) weightsets
do not derive from `weightset_base`.  It would therefore be more precise to
name `weightset_base` as `weightset_wrapper_base`, but that might be
uselessly long.  Names may change again in the future.

Because `dyn::polynomial` and `dyn::weight` store the corresponding
polynomialset/weightset, there is no need (so far?) for a
`dyn::polynomialset`/`dyn::weightset`.  Therefore, `abstract_polynomialset`
and `abstract_weightset` were removed.

For consistency (and many other benefits), `automaton_wrapper` and
`context_wrapper` were introduced.  Now, the whole static API is completely
independent of the dyn API, and the dyn API consists only of wrappers of
"static"-level values---instead of inheritance for automata and contexts.

## 2013-10-25
### renamings (static, dynamic, TAF-Kit)
    derive        -> derivation
    derived-terms -> derived-term
    infiltrate    -> infiltration

## 2013-10-24
### taf-kit: option -q for "quiet"
Equivalent to "-O null": do not generate output.

### star-normal-form: new algorithm (static, dynamic, TAF-Kit)
Compute an equivalent rational expression where starred subexpressions have
a non null constant term.  Yields the same standard automaton, but built
faster.

    $ vcsn star-normal-form -e '(a*b*)*'
    (a+b)*

Valid only for Boolean automata.

### ratexp printing: minimize parentheses
Print rational expressions with the minimum required number of parentheses,
making some large expressions considerably easier to read.  For example

Before:

    $ vcsn expand -C 'law_char(abc)_z' -e '(a+b)?{2}*'
    (\e+<2>a+<2>b+(aa)+(ab)+(ba)+(bb))*
    $ vcsn ladybird 2 | vcsn determinize | vcsn aut-to-exp
    \e+(a.((b+(a.a)+(c.((a+c)*).b))*).(a+(c.((a+c)*))))

After:

    $ vcsn expand -C 'law_char(abc)_z' -e '(a+b)?{2}*'
    (\e+<2>a+<2>b+aa+ab+ba+bb)*
    $ vcsn ladybird 2 | vcsn determinize | vcsn aut-to-exp
    \e+a.(b+a.a+c.(a+c)*.b)*.(a+c.(a+c)*)

### expand: new algorithm (static, dynamic, TAF-Kit)
Given a rational expression, distribute product over addition (recursively
under the starred subexpressions) group and sort the equal monomials.

    $ vcsn expand -C 'lal_char(abc)_z' -e '(a+b)?{2}*'
    (\e+<2>a+<2>b+(a.a)+(a.b)+(b.a)+(b.b))*

----------------------------------------------------------------------

# Vaucanson 2b.0 (2013-10-22)
Release of our first beta, vaucanson-2b.0.

## 2013-10-17
### shuffle, infiltrate: new algorithms (static, dynamic, TAF-Kit)
New algorithms on LAL automata: computes the shuffle-product and
infiltration of two automata.  These (and now product as well) require
weightsets to be commutative semirings.

    $ vcsn standard -C 'lal_char(ab)_z' -E -e 'ab' -o ab.gv
    $ vcsn product    -f ab.gv ab.gv | vcsn enumerate -O text -f - 4
    ab
    $ vcsn shuffle    -f ab.gv ab.gv | vcsn enumerate -O text -f - 4
    <4>aabb + <2>abab
    $ vcsn infiltrate -f ab.gv ab.gv | vcsn enumerate -O text -f - 4
    ab + <2>aab + <2>abb + <4>aabb + <2>abab

## 2013-10-15
### derived-terms: new algorithm (static, dynamic, TAF-Kit)
In addition to thompson and standard, this is a third means to build an
automaton from a (weighted) rational expression.  It corresponds to the
Antimirov definition of derivatives (implemented by "derive").  It requires
LAL rational expressions.

    $ vcsn derived-terms -C 'lal_char(ab)_q' -e '(<1/6>a*+<1/3>b*)*'
    digraph
    {
      vcsn_context = "lal_char(ab)_q"
      rankdir = LR
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I0
        F0
        F1
        F2
      }
      {
        node [shape = circle]
        0
        1
        2
      }
      I0 -> 0
      0 -> F0 [label = "<2>"]
      0 -> 1 [label = "<1/3>a"]
      0 -> 2 [label = "<2/3>b"]
      1 -> F1 [label = "<2>"]
      1 -> 1 [label = "<4/3>a"]
      1 -> 2 [label = "<2/3>b"]
      2 -> F2 [label = "<2>"]
      2 -> 1 [label = "<1/3>a"]
      2 -> 2 [label = "<5/3>b"]
    }

### vcsn eliminate-state
This tool allows one to eliminate states one after the other.

    $ vcsn eliminate-state -f lao.gv 2

Its interface is likely to change, or to be completely removed.

### Smaller libraries
Some compiler magic was used to reduce the size of the libraries (about 20%
on Mac OS X using GCC 4.8).  TAF-Kit might start faster.

## 2013-10-14
### determinize: by default no longer forces the result to be complete
The determinization (static, dynamic, TAF-Kit) now admits an optional second
argument (defaulting to '0'), a Boolean stating whether the result must be
complete.

## 2013-10-12
### polynomialset fully replaces entryset
The 'entryset' type is removed, as polynomialset now provides a strict
superset of its features.

## 2013-10-11
### derive: new algorithm (static, dynamic, TAF-Kit)
New algorithm on LAL automata: computes the derivation of rational
expressions with respect to a word.

    $ vcsn derive -C 'lal_char(a)_z' -e '(<2>a)*' a
    <2>(<2>a)*
    $ vcsn derive -C 'lal_char(a)_z' -e '(<2>a)*' aa
    <4>(<2>a)*
    $ vcsn derive -C 'lal_char(a)_z' -e '(<2>a)*' aaaa
    <16>(<2>a)*
    $ vcsn derive -C 'lal_char(a)_z' -e '(<2>a)*' b
    \z

    $ vcsn derive -C 'lal_char(ab)_q' -e '(<1/6>a*+<1/3>b*)*' a
    <1/3>(a*).(((<1/6>a*)+(<1/3>b*))*)
    $ vcsn derive -C 'lal_char(ab)_q' -e '(<1/6>a*+<1/3>b*)*' aa
    <4/9>(a*).(((<1/6>a*)+(<1/3>b*))*)

## 2013-10-08
### enumerate returns a dyn::polynomial
The 'enumerate' algorithm use to cheat, and returned a std::vector<string>
(each string being a pretty-printing of the monomial, e.g., "<2>a").  It now
returns a dyn::polynomial.

vcsn-enumerate is biased to prefer the 'list' output format:

    $ vcsn enumerate -Ee 'a*' 3
    \e
    a
    aa
    aaa

however the proper syntax for polynomials can be asked for.

    $ vcsn enumerate -O text -Ee 'a*' 3
    \e + a + aa + aaa

### New dyn:: type: polynomial
A new member joins the dyn:: family of types (i.e., automaton, ratexp,
ratexpset, weight, weightset).  Currently there are no means to read such a
value from TAF-Kit, but there is output support with two different output
format:

- 'text' (aka 'default')
    Prints the polynomial this usual way, e.g., "<2>a+<3>b".

- 'list'
    Prints one monomial per line, e.g.
    '<2>a
     <3>b'

## 2013-10-02
### mutable_automaton: speed improvement
"set_transition" used to invoke twice "get_transition", which had a serious
performance impact on some algorithms.  This is fixed.

Before:

     9.08s (0.33s+8.75s): ladybird 21 | determinize -O null
    16.88s (0.85s+16.03s): thompson -C "lan_char(a)_b" -Ee "a?{2000}" | proper -O null
    21.30s (9.76s+11.54s): standard -Ee "(a+b+c+d)?{100}" | aut-to-exp -O null
     7.29s (0.04s+7.25s): standard -C "lal_char(ab)_z" -Ee "(a+b)*b(<2>a+<2>b)*" | power -O null -f- 10
     0.04s (0.04s): standard -E -e "(a?){70}" -o a70.gv
    24.20s (24.20s): product -O null -f a70.gv a70.gv

After:

     8.54s (0.13s+8.41s): ladybird 21 | determinize -O null
    11.87s (0.86s+11.01s): thompson -C "lan_char(a)_b" -Ee "a?{2000}" | proper -O null
    21.03s (9.40s+11.63s): standard -Ee "(a+b+c+d)?{100}" | aut-to-exp -O null
     6.58s (0.06s+6.52s): standard -C "lal_char(ab)_z" -Ee "(a+b)*b(<2>a+<2>b)*" | power -O null -f- 10
     0.07s (0.07s): standard -E -e "(a?){70}" -o a70.gv
    13.16s (13.16s): product -O null -f a70.gv a70.gv

Note in particular that the spontaneous transition elimination algorithm is
faster, going from 16s to 11s on a MacBook Pro i7 2.9GHz 8GB RAM, on the
following sequence.

    $ vcsn thompson -C 'lan_char(a)_b' -Ee 'a?{2000}' | vcsn proper -O null

## 2013-10-01
### random: new algorithm (static, dynamic, TAF-Kit)
Random generation of automata.  Subject to changes.  Accepts four
arguments: number of states, density (of transitions, defaults to .1,
1 generates a clique), number of initial states (defaults to 1), and
number of final states (defaults to 1).

So far LAL and LAN only, no support for random weights.

    $ vcsn random -O fado 3
    @DFA 0
    0 d 1
    1 c 2
    2 d 2
    $ vcsn random -O fado 3
    @DFA 2
    0 c 1
    1 d 2
    2 c 0
    $ vcsn random -C 'lan_char(ab)_b' -O fado 3
    @NFA 1 * 0
    0 @epsilon 1
    1 b 2
    2 b 1

## 2013-09-24
### Overhaul of the package
Thanks to Automake 1.14 features, there is now a single Makefile, which
significantly speeds up the compilation of the package.  The test-suite now
uses the Test Anything Protocol, which results in more verbose results.

Beware that because of subtle issues (in the generated Makefile snippets
that track dependencies), you are highly recommend to "make clean" after
upgrading from the Git repository, and then "make" as usual.

## 2013-09-20
### proper: faster implementation
The spontaneous transition elimination algorithm is now faster, going from
102s to 15s on a MacBook Pro i7 2.9GHz 8GB RAM, on the following sequence.

    $ vcsn thompson -C 'lan_char(a)_b' -Ee 'a?{2000}' | vcsn proper -O null

## 2013-09-17
### is-ambiguous
New algorithm (static, dynamic, TAF-Kit) on LAL automata: whether some word
is the label of at least two successful computations.

    $ vcsn is-ambiguous <<\EOF
    digraph
    {
      vcsn_context="lal_char(ab)_b"
      I -> 0
      0 -> 1 [label = "a"]
      0 -> 2 [label = "a"]
      1 -> F
    }
    EOF
    false

    $ vcsn is-ambiguous <<\EOF
    digraph
    {
      vcsn_context="lal_char(ab)_b"
      I -> 0
      0 -> 1 [label = "a"]
      0 -> 2 [label = "a"]
      1 -> F
      2 -> F
    }
    EOF
    true

Also reported in the 'info' format for automata.

### product: recover the original states
Similarly to 'determinize', the (static version of) 'product' can now be
queried to get a map from states of the result to pairs of original states.

## 2013-09-08
### Sum of standard automata is fixed
See the previous entry: the computation of the initial transition was wrong,
which resulted in the production of non-standard automata.  This is fixed:

    $ vcsn standard -C 'lal_char(a)_ratexpset<lal_char(x)_b>' -e '<x>a*' > 1.gv
    $ vcsn standard -C 'lal_char(b)_ratexpset<lal_char(y)_b>' -e '<y>b*' > 2.gv
    $ vcsn sum -f 1.gv 2.gv
    digraph
    {
      vcsn_context = "lal_char(ab)_ratexpset<lal_char(xy)_b>"
      rankdir = LR
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I0
        F0
        F1
        F2
      }
      {
        node [shape = circle]
        0
        1
        2
      }
      I0 -> 0
      0 -> F0 [label = "<x+y>"]
      0 -> 1 [label = "<x>a"]
      0 -> 2 [label = "<y>b"]
      1 -> F1
      1 -> 1 [label = "a"]
      2 -> F2
      2 -> 2 [label = "b"]
    }

## 2013-09-06
### Operations on automata are generalized
Operations (union, sum, concatenate, chain) were uselessly restricted to
LAL.  Besides, the contexts were improperly computed (both labelset and
weightset).  This is fixed.

    $ vcsn standard -C 'lal_char(a)_ratexpset<lal_char(x)_b>' -e '<x>a*' > 1.gv
    $ vcsn standard -C 'lal_char(b)_ratexpset<lal_char(y)_b>' -e '<y>b*' > 2.gv
    $ vcsn sum -f 1.gv 2.gv
    digraph
    {
      vcsn_context = "lal_char(ab)_ratexpset<lal_char(xy)_b>"
      rankdir = LR
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I0
        F0
        F1
        F2
      }
      {
        node [shape = circle]
        0
        1
        2
      }
      I0 -> 0 [label = "<\\e+\\e+\\e>"]
      0 -> F0 [label = "<x+y>"]
      0 -> 1 [label = "<x>a"]
      0 -> 2 [label = "<y>b"]
      1 -> F1
      1 -> 1 [label = "a"]
      2 -> F2
      2 -> 2 [label = "b"]
    }

## 2013-09-04
### double-ring
New algorithm (static, dynamic, TAF-Kit).

    $ vcsn double-ring -C 'lal_char(ab)_b' 6 1 3 4 5
    digraph
    {
      vcsn_context = "lal_char(ab)_b"
      rankdir = LR
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I0
        F1
        F3
        F4
        F5
      }
      {
        node [shape = circle]
        0
        1
        2
        3
        4
        5
      }
      I0 -> 0
      0 -> 1 [label = "a"]
      0 -> 5 [label = "b"]
      1 -> F1
      1 -> 0 [label = "b"]
      1 -> 2 [label = "a"]
      2 -> 1 [label = "b"]
      2 -> 3 [label = "a"]
      3 -> F3
      3 -> 2 [label = "b"]
      3 -> 4 [label = "a"]
      4 -> F4
      4 -> 3 [label = "b"]
      4 -> 5 [label = "a"]
      5 -> F5
      5 -> 0 [label = "a"]
      5 -> 4 [label = "b"]
    }

It is advised to pass "layout = circo" to Dot for the rendering.

### concatenation, chain
New algorithm on standard automata (static, dynamic, TAF-Kit).

### right-mult
New algorithm on standard automata (static, dynamic, TAF-Kit).  Same
limitations as left-mult, see below.

## 2013-09-03
### left-mult
New algorithm on standard automata (static, dynamic, TAF-Kit).

The TAF-Kit version is (currently) troublesome, as it does not infer the
context to parse the weight from the automaton: be sure to specify -C:

    $ vcsn standard -C 'lal_char(a)_z' -e a | vcsn left-mult 12
    vcsn-left-mult: left_mult: no implementation available for mutable_automaton<lal_char_z> x b
    $ vcsn standard -C 'lal_char(a)_z' -e a | vcsn left-mult -C 'lal_char(a)_z' 12
    digraph
    {
      vcsn_context = "lal_char(a)_z"
      rankdir = LR
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I0
        F1
      }
      {
        node [shape = circle]
        0
        1
      }
      I0 -> 0
      0 -> 1 [label = "<12>a"]
      1 -> F1
    }

### taf-kit: option -w for weights as input
Currently only vcsn-cat supports it.

    $ vcsn cat -w -e 2
    vcsn-cat: invalid Boolean: 2
    $ vcsn cat -w -e 1
    1
    $ vcsn cat -C 'lal_char(a)_z' -w -e 2
    2

### concatenate, star, sum
New algorithms on standard automata (static, dynamic, TAF-Kit).

## 2013-09-02
### union
New algorithm on automata (static as "union_a", dynamic as "union_a",
TAF-Kit as "union").  Plain graph union.

### is-valid
New algorithm on rational expressions (static, dynamic, TAF-Kit).

    $ vcsn is-valid -C 'lal_char(a)_r' -E -e '(<.5>\e)*'
    true
    $ vcsn is-valid -C 'lal_char(a)_r' -E -e '\e*'
    false

For consistency, is-valid is now also available for automata in dyn:: and
TAF-Kit (it used to be static only, visible from "info" output).

    $ vcsn thompson -C 'lan_char(a)_r' -e '(<.5>\e)*' | vcsn is-valid
    true
    $ vcsn thompson -C 'lan_char(a)_r' -e '\e*' | vcsn is-valid
    false

## 2013-08-13
### More RatExp quantifiers
In addition to "*", there is "?"/"{?}" and "{+}".  Support for "{*}" is
added for symmetry.

    $ vcsn cat -Ee 'a?'
    \e+a
    $ vcsn cat -Ee 'a?{3}'
    (\e+a).(\e+a).(\e+a)
    $ vcsn cat -Ee '(a+b){+}'
    (a+b).((a+b)*)

## 2013-08-02
### I/O EFSM format support
We are now able to produce and read EFSM format (designed as an interface to
OpenFST).  This is not (yet) thoroughly tested for weighted automata.

    $ vcsn ladybird -O efsm 4 |
       efstcompile | fstdeterminize | efstdecompile |
       vcsn cat -I efsm -O info
    type: mutable_automaton<lal_char(abc)_b>
    number of states: 15
    number of initial states: 1
    number of final states: 8
    number of accessible states: 15
    number of coaccessible states: 15
    number of useful states: 15
    number of transitions: 43
    number of deterministic states: 15
    number of eps transitions: 0
    is complete: 0
    is deterministic: 1
    is empty: 0
    is eps-acyclic: 1
    is normalized: 0
    is proper: 1
    is standard: 0
    is trim: 1
    is useless: 0
    is valid: 1

### Output in EFSM format is improved
State numbers now start appropriately at 0 (instead of 2), and when there is
a single initial state, no "pre-initial state" is output; this avoids the
introduction of spontaneous transitions in deterministic automata.

Before (see news of 2013-07-01):

    $ vcsn ladybird -O efsm 2
    #! /bin/sh

    cat >transitions.fsm <<\EOFSM
    0       2       \e
    2
    2       3       a
    3       3       b
    3       3       c
    3       2       c
    3       2       a
    EOFSM

    cat >isymbols.txt <<\EOFSM
    \e      0
    a       1
    b       2
    c       3
    EOFSM

    fstcompile --acceptor --keep_isymbols --isymbols=isymbols.txt transitions.fsm

Now:

    $ vcsn ladybird -O efsm 2
    #! /bin/sh

    cat >isymbols.txt <<\EOFSM
    \e      0
    a       1
    b       2
    c       3
    EOFSM

    cat >transitions.fsm <<\EOFSM
    0       1       a
    1       0       a
    1       1       b
    1       0       c
    1       1       c
    0
    EOFSM

    fstcompile --acceptor --keep_isymbols --isymbols=isymbols.txt transitions.fsm

### I/O FAdo format support
We are now able to produce and read FAdo format.

    $ vcsn ladybird -O fado 4 | \
      python -c "from FAdo import fa
    nfa = fa.readFromFile('/dev/stdin')[0]
    dfa = nfa.toDFA()
    fa.saveToFile('dl4.fado', dfa)"

    $ vcsn cat -I fado -O info -f dl4.fado
    type: mutable_automaton<lal_char(abc)_b>
    number of states: 15
    number of initial states: 1
    number of final states: 8
    number of accessible states: 15
    number of coaccessible states: 15
    number of useful states: 15
    number of transitions: 43
    number of deterministic states: 15
    number of eps transitions: 0
    is complete: 0
    is deterministic: 1
    is empty: 0
    is eps-acyclic: 1
    is normalized: 0
    is proper: 1
    is standard: 0
    is trim: 1
    is useless: 0
    is valid: 1

### constant-term
New algorithm on rational expressions (static, dynamic, TAF-Kit).

    $ vcsn constant-term -e '(?@lal_char(a)_b)(\e)*'
    1
    $ vcsn constant-term -e '(?@lal_char(a)_z)(\e)*'
    vcsn-constant-term: z: star: invalid value: 1
    $ vcsn constant-term -C 'law_char(ab)_ratexpset<law_char(wxyz)_b>' \
                         -e '<w>(<x>a*+<y>b*)*<z>'
    w.((x+y)*).z

## 2013-07-26
### Automaton library
The set of library automata for existing contexts is now complete with
respect to what Vaucanson 1 provided.  Automata families are not, and will
not, be part of this library ; for instance, instead of looking for
ladybird-6.gv, use 'vcsn ladybird 6'.

    lal_char_b: a1.gv b1.gv evena.gv oddb.gv
    lal_char_z: b1.gv binary.gv c1.gv d1.gv
    lal_char_zmin: minab.gv minblocka.gv slowgrow.gv

Currently one must specify their path:

    $ vcsn evaluate -f share/vcsn/lal_char_zmin/minab.gv aabbba
    3

## 2013-07-25
### TAF-Kit: works on standard input by default
The very frequent "-f -" sequence is no longer required: by default the
input is stdin.

    $ vcsn ladybird 2 | vcsn determinize | vcsn aut-to-exp
    \e+(a.((b+(a.a)+(c.((a+c)*).b))*).(a+(c.((a+c)*))))

## 2013-07-18
### standard
New algorithm on automata (static, dynamic, TAF-Kit).  Corresponds to
"standardize" in Vaucanson 1.

### is-standard
New algorithm on automata (static, dynamic, TAF-Kit).  Also reported in
"info" format.

### u
New automata factory (static, dynamic, TAF-Kit): Brzozowski's universal
witness.

## 2013-07-17
### New formats: grail and fado

    $ vcsn standard -O fado -e 'a+b'
    @DFA 1 2
    0       a       1
    0       b       2
    $ vcsn standard -O fado -e 'a+ab'
    @NFA 1 3 * 0
    0       a       1
    2       b       3
    0       a       2
    $ vcsn standard -O grail -e 'a+ab'
    (START)     |-      0
    0       a       1
    2       b       3
    0       a       2
    1       -|      (FINAL)
    3       -|      (FINAL)

## 2013-07-14
### power
New algorithm on automata (static, dynamic, TAF-Kit).

    $ vcsn standard -e '(?@lal_char(01)_z)(0+1)*1(<2>0+<2>1)*' >binary.gv
    $ vcsn power -f binary.gv 0 | vcsn enumerate -f- 2
    \e
    0
    1
    00
    01
    10
    11
    $ vcsn power -f binary.gv 1 | vcsn enumerate -f- 2
    1
    <2>10
    <3>11
    $ vcsn power -f binary.gv 2 | vcsn enumerate -f- 2
    1
    <4>10
    <9>11
    $ vcsn power -f binary.gv 4 | vcsn enumerate -f- 2
    1
    <16>10
    <81>11
    $ vcsn power -f binary.gv 8 | vcsn enumerate -f- 2
    1
    <256>10
    <6561>11

## 2013-07-13
### divkbaseb
New algorithm (static, dynamic, TAF-Kit).

    $ vcsn divkbaseb -C 'lal_char(01)_b' 3 2
    digraph
    {
      vcsn_context = "lal_char(01)_b"
      rankdir = LR
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I0
        F0
      }
      {
        node [shape = circle]
        0
        1
        2
      }
      I0 -> 0
      0 -> F0
      0 -> 0 [label = "0"]
      0 -> 1 [label = "1"]
      1 -> 0 [label = "1"]
      1 -> 2 [label = "0"]
      2 -> 1 [label = "0"]
      2 -> 2 [label = "1"]
    }

## 2013-07-12
### enumerate produces a list of weighted words
enumerate now also provides the weight of the words.  It is also fixed: it
no longer reports words with nul weight (e.g., 'a' in 'a+<-1>a').

    $ vcsn standard -e '(?@lal_char(01)_z)(0+1)*1(<2>0+<2>1)*' \
        | vcsn enumerate -f - 3
    1
    01
    <2>10
    <3>11
    001
    <2>010
    <3>011
    <4>100
    <5>101
    <6>110
    <7>111

shortest is fixed and modified similarly.

    $ vcsn standard -e '(?@lal_char(ab)_z)(a+<5>bb+<-1>a)' \
          | vcsn shortest -f -
    <5>bb

## 2013-07-04
### WeightSet: added support for rational weights
We can now use automata with rational numbers as their weights.

    $ vcsn standard -e "(?@lal_char(ab)_q)(<1/2>a+<2>b)*" \
      | vcsn evaluate -f - aaabbbb
    2

    $ vcsn standard -e "(?@lal_char(ab)_q)(<1/2>a+<2>b)*"\
      | vcsn evaluate -f - aaab
    1/4

## 2013-07-01
### new format: efsm (and new tool: efstcompile)
The former output format ("fsm") is dropped, replaced by an adhoc "extended
FSM" format: "efsm".  The FSM format focuses only on the transitions, and
lacks information about the labels (which are expected to be mapped to
numbers), weights, whether it's an acceptor or transducer, etc.

The efsm format is designed to be simple to use with OpenFST: just run
"efstcompile" instead of "fstcompile".  As a matter fact, the new
"efstcompile" tool is rather dumb, as it simply executes the "efsm" file.

    $ vcsn ladybird -O efsm 2
    #! /bin/sh

    cat >transitions.fsm <<\EOFSM
    0       2       \e
    2
    2       3       a
    3       3       b
    3       3       c
    3       2       c
    3       2       a
    EOFSM

    cat >isymbols.txt <<\EOFSM
    \e      0
    a       1
    b       2
    c       3
    EOFSM

    fstcompile --acceptor --keep_isymbols --isymbols=isymbols.txt transitions.fsm

    $ vcsn ladybird -O efsm 8 | efstcompile | fstdeterminize | fstinfo \
        | grep '# of states'
    # of states                                       256

## 2013-06-28
### new binary: vcsn
To give a flavor of what TAF-Kit should be (a single tool instead of one per
command), the new "vcsn" script bounces to the vcsn-* tools.  It does not
support \| as TAK-Kit 1 did.

    $ vcsn are-equivalent -Ee '(a*b*)*' '(a+b)*'
    true

## 2013-06-26
### proper
Now removes states to which no transition arrive after spontaneous
transitions removal.

### Thompson
Only the concatenation yielded an automaton whose projection on Boolean
weights was different from the Thompson of the projection of the rational
expression on Boolean.  This is now fixed.

### is-normalized
New algorithm (static, dynamic, TAF-Kit).

## 2013-06-25
### identity, unit => one
Labels new define one() and is_one() instead of identity() and
is_identity().

We now use LAO, "labels are one", instead of LAU, "labels are unit".

WeightSets now define one() and is_one() instead of unit() and is_unit().

## 2013-06-21
### shortest, enumerate
New algorithms (static, dynamic, TAF-Kit).

## 2013-06-20
### universal
Now accepts (LAL Boolean) rational expressions.

### are-equivalent
Now accepts (LAL Boolean) rational expressions.  It cannot compare a
rational expression with an automaton (or vice-versa).  This is a temporary
defect which shell be addressed once TAF-Kit is properly developed.

    $ vcsn-are-equivalent -Ee '(a*b*)*' '(a+b)*'
    true
    $ vcsn-are-equivalent -Ee '(a*b)*' '(a+b)*'
    false

### dyn: overhaul
Consistency is enforced in dyn.  In particular the very first dynamic/static
bridge (which was not identified as such), vcsn::rat::abstract_ratexpset, is
now part of dyn::.

dyn::weight now aggregates its WeightSet instead of a Context.  More similar
conversions were performed, and other are to come.

## 2013-06-19
### dot
The output now shows useless states (and their transitions) in gray.

## 2013-06-18
### are-equivalent
New algorithm (static, dynamic, TAF-Kit).  Currently works only for LAL
Boolean automata.

    $ vcsn-standard -e 'a(ba)*' -o a1.gv
    $ vcsn-standard -e '(ab)*a' -o a2.gv
    $ vcsn-are-equivalent -f a1.gv a2.gv
    true

## 2013-06-17
### is_trim, is_useless, is_empty
New algorithms (static, dynamic, TAF-Kit).

### accessible_states, coaccessible_states, useful_states
New algorithms, static only.

### num_accessible_states, num_coaccessible_states, num_useful_states
New algorithms, static only.  Available in the "info" display.

### copy accepts a predicate
It is now possible to filter the states to keep.  Either as a predicate, or
a set of states.  For instance:

    template <typename Aut>
    Aut trim(const Aut& a)
    {
      return vcsn::copy(a, useful_states(a));
    }

### universal
New algorithm (static, dynamic, and TAF-Kit).  Requires a LAL Boolean
automaton.

    $ vcsn-universal -f a1.gv
    digraph
    {
      vcsn_context = "lal_char(ab)_b"
      rankdir = LR
      node [shape = circle]
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I0
        F2
      }
      { 0 1 2 }
      I0 -> 0
      0 -> 0 [label = "a, b"]
      0 -> 1 [label = "a"]
      1 -> 0 [label = "a, b"]
      1 -> 1 [label = "a, b"]
      1 -> 2 [label = "b"]
      2 -> F2
      2 -> 0 [label = "a, b"]
      2 -> 1 [label = "a, b"]
      2 -> 2 [label = "a, b"]
    }

### complement
New algorithm (static, dynamic, and TAF-Kit).  Requires a complete
deterministic LAL Boolean automaton.

    $ vcsn-standard -e '(?@lal_char(ab)_b)a' \
        | vcsn-determinize -f- \
        | vcsn-complement -f-  \
        | vcsn-aut-to-exp -f-
    \e+((b+(a.(a+b))).((a+b)*))

## 2013-06-13
### is-deterministic
It is conforming with the specifications: _all_ the states must be
deterministic, not just the reachable ones.

### info output adjustments
It now displays whether the automaton "is complete" and the number of
deterministic states if LAL, otherwise "N/A".  It also reports "is
deterministic: N/A" for non-LAL.

### Invalid labels are rejected
Under some circumstances, some invalid transitions could be accepted by the
Dot parser (e.g., "aa" or "\e" in LAL).  This is fixed.

## 2013-06-05
### Support for entries is removed.
Member types, functions, and variables, about entries, have been removed.
The services provided by entries in Vaucanson 1 are provided by LAU
automata, so entries are not as useful in Vaucanson 2.  And anyway, if
needed, it should rather be a set of free standing functions.

## 2013-05-27
### Refactoring: eps-removal => proper
Eps-removal is renamed to Proper.

## 2013-05-24
### Syntax for rational-expressions has changed!
Angular brackets are now used for weights.  Instead of

    $ vcsn-standard -e '(?@lal_char(01)_z)(0+1)*1({2}1+{2}0)*' -o binary.dot

run

    $ vcsn-standard -e '(?@lal_char(01)_z)(0+1)*1(<2>1+<2>0)*' -o binary.dot

Braces are now used instead of (*...) for generalized quantifiers.

    a{0} => \e
    a{1} => a
    a{2} => a.a
    a{5} => a.a.a.a.a

    a{0,1} => \e+a
    a{0,2} => \e+a+a.a
    a{0,3} => \e+a+a.a+a.a.a

    a{1,2} => a.(\e+a
    a{1,3} => a.(\e+a+a.a)

    a{2,5} => a.a.(\e+a+a.a+a.a.a)

    a{0,} => a*
    a{1,} => a.(a*)
    a{4,} => a.a.a.a.(a*)

## 2013-05-23
### info output
Now displays the number of spontaneous transitions (0 for LAL).

## 2013-04-29
### Digits as letters
Recently broken by accident, support for digits as letters is restored.

    $ vcsn-standard -e '(?@lal_char(01)_z)(0+1)*1({2}1+{2}0)*' -o binary.dot
    $ vcsn-evaluate -f binary.dot '11111111'
    255
    $ vcsn-evaluate -f binary.dot '101010'
    42

## 2013-04-26
### Dot parser
This parser is now stricter than it used to be: be sure to escape
backslashes in input Dot files, i.e.,

      0 -> 2 [label = "\e"]

is now invalid, write

      0 -> 2 [label = "\\e"]

This change was made because Graphviz treats "\e" (and renders it) exactly
like "e".

## 2013-04-25
### New algorithm: Thompson
Conversion from rational expression to automata.  Requires lan or law.
The handling of weights might be changed in the near future.

    $ vcsn-thompson -Ee '(?@lan_char(abc)_z){2}(a+{3}b+c)*{5}'
    digraph
    {
      vcsn_context = "lan_char(abc)_z"
      rankdir = LR
      node [shape = circle]
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I8
        F9
      }
      { 0 1 2 3 4 5 6 7 8 9 }
      I8 -> 8
      0 -> 2 [label = "\\e"]
      0 -> 4 [label = "\\e"]
      0 -> 6 [label = "\\e"]
      1 -> 0 [label = "\\e"]
      1 -> 9 [label = "{5}\\e"]
      2 -> 3 [label = "a"]
      3 -> 1 [label = "\\e"]
      4 -> 5 [label = "{3}b"]
      5 -> 1 [label = "\\e"]
      6 -> 7 [label = "c"]
      7 -> 1 [label = "\\e"]
      8 -> 0 [label = "{2}\\e"]
      8 -> 9 [label = "{10}\\e"]
      9 -> F9
    }

## 2013-04-22
### standard is the new name for standard-of
The "of" is useless, inconsistent with the other algorithms, and with the
TAF-Kit v1 name.

## 2013-04-18
### Evaluation is fixed
Several initializations were incorrect, expecting the zero to be 0 (which is
not the case for zmin for instance).  There might also be some speed up.

## 2013-04-16
### New output format: tikz
Layout is dumb, yet this is useful to prepare LaTeX documents.

## 2013-04-12
### New output format: info
This pseudo format displays facts about the automaton (number of states and
so on) or rational expressions (number of nodes).

### is-eps-acyclic
Can now be called on LAL automata, for consistency with is-proper and
is-valid.

## 2013-04-11
### Dot
The states are now numbered from 0.

### Clang Compatibility
Clang++ (3.2 and 3.3) can now compile Vaucanson.

## 2013-03-18
### New LabelSet: nullableset
Initial support for "Labels are nullable".

    $ cat lan.dot
    digraph lan
    {
      vcsn_context = "lan_char(a)_b"
      I1 -> 1
      2 -> F2
      1 -> 2 [label = "a"]
      1 -> 2 [label = "\e"]
    }
    $ vcsn-cat -Af lan.dot
    digraph
    {
      vcsn_context = "lan_char(a)_b"
      rankdir = LR
      node [shape = circle]
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I1
        F2
      }
      { 1 2 }
      I1 -> 1
      1 -> 2 [label = "\\e, a"]
      2 -> F2
    }
    $ vcsn-eps-removal -Af lan.dot
    digraph
    {
      vcsn_context = "lan_char(a)_b"
      rankdir = LR
      node [shape = circle]
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I1
        F2
        F1
      }
      { 1 2 }
      I1 -> 1
      1 -> F1
      1 -> 2 [label = "a"]
      2 -> F2
    }

### is-proper
New dyn algorithm and tool (vcsn-is-proper).

## 2013-03-08
### determinize speed-up
Determinization algorithm is five times faster.

Before:

    $ time bin/vcsn-de-bruijn 18 | bin/vcsn-determinize -Af - -Onull
    real    0m13.049s
    user    0m12.773s
    sys     0m0.276s
    $ time bin/vcsn-de-bruijn 20 | bin/vcsn-determinize -Af - -Onull
    real    0m56.780s
    user    0m55.775s
    sys     0m0.988s

Now:

    $ time bin/vcsn-de-bruijn 18 | bin/vcsn-determinize -Af - -Onull
    real     0m2.181s
    user     0m2.048s
    sys      0m0.132s
    $ time bin/vcsn-de-bruijn 20 | bin/vcsn-determinize -Af - -Onull
    real     0m8.653s
    user     0m8.177s
    sys      0m0.476s

## 2013-02-26
### hierarchy and API clean up
dyn:: was cleaned.  Some headers were renamed:

    vcsn/ctx/abstract_context.hh -> vcsn/dyn/context.hh
    vcsn/algos/dyn.hh            -> vcsn/dyn/algos.hh
    vcsn/core/automaton.hh       -> vcsn/dyn/automaton.hh

dyn::make_automaton was introduced to hide implementation details.

## 2013-02-21
### LabelSet renamings
The name LetterSet, UnitSet, and WordSet were not compliant.  They have been
renamed as letterset, unitset, and wordset.

## 2013-02-20
### Generalized quantifier as syntactic sugar for rational expressions
The new "(* min, max)" quantifier (postfix, like "*") allows to specify
"powers" of an expression.  For instance:

    ({a}b)(*0) => \e
    ({a}b)(*1) => {a}b
    ({a}b)(*2) => {a}b.{a}b
    ({a}b)(*5) => {a}b.{a}b.{a}b.{a}b.{a}b

    ({a}b)(*0,1) => \e+{a}b
    ({a}b)(*0,2) => \e+{a}b+({a}b.{a}b)
    ({a}b)(*0,3) => \e+{a}b+({a}b.{a}b)+({a}b.{a}b.{a}b)

    ({a}b)(*1,2) => {a}b.(\e+{a}b)
    ({a}b)(*1,3) => {a}b.(\e+{a}b+({a}b.{a}b))

    ({a}b)(*2,5) => {a}b.{a}b.(\e+{a}b+({a}b.{a}b)+({a}b.{a}b.{a}b))

    ({a}b)(*0,) => ({a}b)*
    ({a}b)(*1,) => {a}b.(({a}b)*)
    ({a}b)(*4,) => {a}b.{a}b.{a}b.{a}b.(({a}b)*)

## 2013-02-18
### Comment in rational expressions
The (?#...) construct allows to embed comments in rational expressions.
They are discarded.  There is no means to include a closing parenthesis in
this construct.

### Context in rational expressions
The (?@...) allows a rational expression to "carry" its context.  Contrast
for instance the two following runs.

    $ vcsn-cat -C 'lal_char(xyz)_z'  -Ee '{42}x+{51}z'
    {42}x+{51}z
    $ vcsn-cat -Ee '(?@lal_char(xyz)_z){42}x+{51}z'
    {42}x+{51}z

This will be used, eventually, so that TAF-Kit-like tools propagate the
context in runs such as:

    $ vcsn-cat -Ee '(?@lal_char(xyz)_z){42}x+{51}z' | vcsn-transpose -Ef -
    1.1-5: invalid Boolean: 42

Currently it fails, as the "default" context is "lal_char(abc)_b".

### New output format: fsm
An initial, and rough, support for Open FSM's format is provided.  Currently
there is no support _at all_ for the weights.

    $ vcsn-de-bruijn -O fsm 12 | fstcompile | fstdeterminize | wc -l
    1216

### New output format: null
The output is discarded.  This is useful for benching.

## 2013-02-14
### New algorithms: is-deterministic and complete
is-deterministic takes an automaton as argument and exits with code status 0
if the given automaton is deterministic, 2 otherwise.

complete also takes an automaton as argument and make it complete. If the
given automaton is already complete, then it is unchanged.

## 2013-02-12
### Overhaul of the LAU, LAL, LAW implementation
So far a context was a triple: <Kind, LabelSet, WeightSet>, where (for
instance), Kind is labels_are_letters, LabelSet is
set_alphabet<char_letters>, and WeightSet is zmin).  This is troublesome on
several regards, the clearest being that the LabelSet makes no sense for
LAU.

Now contexts are pairs: <LabelSet, WeightSet>, where this time the LabelSet
(same name as before, different concept) can be a instance of UnitSet for
LAU, WordSet for LAW, or LetterSet for LAL.  These structures, in turn, are
parameterized by the effective set of generators to use: for instance,
LetterSet<set_alphabet<char_letters>>.  Of course UnitSet is not
parameterized.

As a first visible consequence, the name of the LAU contexts has changed:

    lau_char_br => lau_br
    lau_char(xyz)_ratexpset<lal_char(abc)_b> => lau_ratexpset<lal_char(abc)_b>

## 2013-01-24
### options renamed
The options to select the input and output format are renamed -I and -O
(instead of -i and -o).

### new option: -o for output file
The vcsn-* tools now support '-o FILE' to save the output in FILE.

## 2013-01-22
### product: strengthened preconditions
The product of automata requires LAL automata.  The output alphabet is the
intersection of the ones of the operands.  This works as expected when the
automata have disjoint alphabets.

## 2013-01-18
### dyn::product and vcsn-product
They compute the product of automata.

## 2013-01-14
### vcsn-determinize and vcsn-evaluate use the common command line options
These tools support -C, -g, etc. like the other tools.  See "vcsn-<tool> -h".

### context names are now complete
Context names used to describe the "static" structure only (e.g.,
`lal_char_ratexpset<law_char_b>`).  It now includes the "dynamic" part,
currently only the list of generators (e.g.,
`lal_char(abc)_ratexpset<law_char(xyz)_b>`).

This context strings are both printed and read by the various tools.  For
instance:

    $ vcsn-standard-of -C 'law_char(xyz)_ratexpset<law_char(abc)_z>' \
          -e '{abc}xyz' | vcsn-aut-to-exp -f -
    {abc}xyz

Note that the second tool, vcsn-aut-to-exp, found the context in its input,
the standard automaton in Dot format.

## 2013-01-13
### dyn::ratexpset, dyn::context
These are now handled by shared pointer, consistently with dyn::automaton
and dyn::ratexp.

Nasty memory management issues have been fixed.

### vcsn-de-bruijn
It now supports the same arguments as the other vcsn-* tools.  It also no
longer requires 'a' and 'b' to be accepted letters, and it uses the whole
alphabet.  For instance "vcsn-de-bruijn -g 'xyz' 3" generates an automaton
for "(x+y+z)x(x+y+z)^3".

### dyn::ladybird, vcsn-ladybird
New dynamic algorithm, and new tool (which also supports the common command
line options).

## 2013-01-12
### contexts are renamed
Contexts have both a name and an identifier.  The name is used to display in
a readable form the nature of the context, for instance in Dot output.  The
identifier is used for instance headers, or predefined contexts, and
libraries.

So far names and identifiers are equal, but this will change.

As a first step, identifiers/names are now <Kind>_<LabelSet>_<WeightSet>
instead of <LabelSet>_<WeightSet>_<Kind>.  For instance:

    char_b_lal  => lal_char_b
    char_br_lal => lal_char_br
    char_zr_lal => lal_char_zr
    char_br_lau => lau_char_br
    char_br_law => law_char_br
    char_zr_law => law_char_zr

## 2013-01-11
### pprat is removed
It was designed for the test suite.  The vcsn-* tools are now sufficient for
the test suite, and are exposed to the user.

### vcsn-*: option overhaul
The different tools had already too many different calling conventions.
They are now (quite) consistent.

### vcsn-aut-to-exp
Calls the default implementation of aut-to-exp.

    $ vcsn-standard-of -Wz     -e '{2}(ab){3}' | vcsn-aut-to-exp -Af -
    ({2}a.b){3}
    $ vcsn-standard-of -Wz -Lw -e '{2}(ab){3}' | vcsn-aut-to-exp -Af -
    {6}ab

### vcsn-lift
It now also supports lifting rational expressions.

    $ vcsn-lift -C char_b_lal -Ee 'abc'
    {a.b.c}\e

## 2013-01-10
### dotty -> dot
The name "dotty" was incorrect (as it denotes a program instead of the
format).  Therefore, every occurrence of "dotty" is now mapped to "dot".

### dot
The Dot output (input and output) now uses ", " as a label separator,
instead of " + ".

## 2012-12-26
### dyn: input/output
Routines: dyn::read_(automaton|ratexp)_(file|string) and dyn::print take the
input format.  Available input/output are:
- "dotty".
- "text".
- "xml".

## 2012-12-19
### genset is replaced by labelset
Through out the code.

## 2012-12-18
### dyn: input
New routines: dyn::read_(automaton|ratexp)_(file|string).

### dyn: output
New routines: dyn::print, for both automata and RatExps.

### bin: new tools
vcsn-cat, vcsn-transpose (both on RatExps only currently).
vcsn-standard-of.
vcsn-lift.

## 2012-12-13
### krat -> rat
kratexp, kratexpset etc. are renamed as ratexp, ratexpset, etc.

### labels are unit
labels-are-empty/lae were mapped to labels-are-unit/lau.

## 2012-10-31
### dyn::context
For consistency with dyn::automaton, vcsn::ctx::abstract_context is renamed
vcsn::dyn::context.  Eventually, we might turn it into a shared pointer too.

### dyn::de_bruijn, bin/vcsn-de-bruijn
New tools, useful for tests for instance.

    $ vcsn-de-bruijn char_b_lal 2
    digraph
    {
      vcsn_context = "char_b_lal"
      vcsn_genset = "ab"
      rankdir = LR
      node [shape = circle]
      {
        node [style = invis, shape = none, label = "", width = 0, height = 0]
        I1
        F4
      }
      I1 -> 1
      1 -> 1 [label = "a + b"]
      1 -> 2 [label = "a"]
      2 -> 3 [label = "a + b"]
      3 -> 4 [label = "a + b"]
      4 -> F4
    }

## 2012-10-22
### dyn::parse_file and parse_string
They construct dyn::automaton's.

### dyn::eval
For bad reasons, currently works only for char_b_lal

### vcsn-determinize and vcsn-evaluate
Two new shell commands to write tests.

## 2012-10-12
### pprat works with abstract algorithms
pprat now uses *only* abstract (aka, dynamic) algorithms!  On OS X, it is
now a 77KB program; it was 11MB before.

This schedules its death: either it will be replaced by a set of smaller
grain commands (vcsn-determinize, vcsn-standard-of, etc.) from which test
cases will be easier to write, or it will be replaced by some early
implementation of a TAF-Kit-like *unified* program (instead of one per
context).

## 2012-10-09
### automata provide a vname
To dispatch algorithms such as dotty, we not only need to know the context
type name, but also the automaton type name, as `mutable_automaton` and
`transpose_automaton` are two different types for instance.

## 2012-10-08
### dot-parser
Because it uses only algorithms made abstract (`make_context`,
`make_automaton_editor`, and `dotty`), the dot parser now works for any of the
precompiled contexts!

## 2012-09-26
### add_entry
In addition to the add_entry method of mutable_automaton, there is now an
`add_entry` _algorithm_, which is templated by the automaton type.  This
algorithm provides an abstract interface to an unknown type of automaton.

## 2012-09-24
### renamings
For consistency, polynomials is renamed polynomialset.

### mutable_automaton::add_entry and del_entry
The first of these new functions allows to add directly a list of transition
between two states by passing the corresponding `entry_t` (this is most useful
when reading an automaton with entries, such as with the Dot parser).  The
second one removes every existing transition between two states.

## 2012-08-01
### labels are empty
Initial work on labels-are-empty automata.  See the unit/char_z_lae test.
The labels are not displayed, but the "{...}" to denote the weights, are
kept:

    digraph
    {
      vcsn_context=char_z_lae
      vcsn_genset=""
      rankdir=LR
      node [shape=circle]
      {
        node [style=invis,shape=none,label="",width=0,height=0]
        I1
        F2
      }
      I1 -> 1
      2 -> F2 [label="{10}"]
      1 -> 2 [label="{51}"]
      2 -> 3 [label="{3}"]
      2 -> 1
      1 -> 1 [label="{42}"]
      1 -> 3
    }

In that case, the transitions do not store labels.

### lift now returns a labels-are-empty automaton/ratexp
Accordingly, pprat -l (lift) now displays:

    $ pprat -Lw -l 'ab+cd'
    digraph
    {
      vcsn_context=char_kratexpset<char_b_law>_lae
      vcsn_genset="abcd"
      rankdir=LR
      node [shape=circle]
      {
        node [style=invis,shape=none,label="",width=0,height=0]
        I1
        F2
        F3
      }
      1 -> 2 [label="{ab}"]
      2 -> F2
      I1 -> 1
      3 -> F3
      1 -> 3 [label="{cd}"]
    }

## 2012-07-31
### dot-parser
It is now possible to load an automaton from its dotty output.  Actually, it
is possible to write simpler automata.  This is no yet fully generic: it
works properly only for `char_b_lal`.

The test program unit/parse-dot gives access to it.  When fed with the
following input file:

    digraph
    {
      vcsn_context=char_b_lal vcsn_genset="a"
      {1} -> {2 3} -> {4 5 6} [label=a]
      I -> 1
      {4 5 6} -> F
    }

it produces an automaton, and dumps it using the dotty algorithm:

    digraph
    {
      vcsn_context=char_b_lal
      vcsn_genset="a"
      rankdir=LR
      node [shape=circle]
      {
        node [style=invis,shape=none,label="",width=0,height=0]
        I1
        F4
        F5
        F6
      }
      1 -> 2 [label="a"]
      1 -> 3 [label="a"]
      2 -> 4 [label="a"]
      2 -> 5 [label="a"]
      2 -> 6 [label="a"]
      3 -> 4 [label="a"]
      3 -> 5 [label="a"]
      3 -> 6 [label="a"]
      I1 -> 1
      4 -> F4
      5 -> F5
      6 -> F6
    }

## 2012-07-25
### pprat uses -L for labels instead of -A
For consistency, since we now also use the name "labels" to denote the
leaves of rational expressions (others that \z and \e), -A is renamed -L.

### Metadata are embedded in the Dot file
The pseudo name "A" which was used in every dotty output is no longer
defined, as it is both optional and useless.  The context name and the
alphabet are also provided.  For instance:

    $ ./tests/unit/ladybird-b 2 | sed 4q
    digraph
    {
      vcsn_context=char_b_lal
      vcsn_genset="abc"
    $ pprat -s -L z 'abc' | sed 4q
    digraph
    {
      vcsn_context=char_z_lal
      vcsn_genset="abcd"
    $ pprat -s -L zr -A w 'abc' | sed 4q
    digraph
    {
      vcsn_context=char_kratexpset<char_z_law>_law
      vcsn_genset="abcd"

This is an experimentation, and the current choice is somewhat
unsatisfactory.  Instead of

      vcsn_context=char_b_lal
      vcsn_genset="abc"

it is probably more sensible to use

      vcsn_context="char_b_lal{abc}"

or maybe

      vcsn_context="char{abc}_b_lal"

so that when weightsets depend for instance upon an alphabet, it can be
specified too.  Instead of

      vcsn_context=char_kratexpset<char_z_law>_law
      vcsn_genset="abcd"

(which does not define the alphabet used for the weightset), one would
expect:

      vcsn_context=char_kratexpset<char_z_law{xyz}>_law{abcd}

or maybe

      vcsn_context=char{abcd}_kratexpset<char{xyz}_z_law>_law

Also, the name "kratexpset" is of course open to discussion:

      vcsn_context=char_rat<char_z_laz{xyw}>_law{abcd}

### polynomials::conv
It is now possible to read back polynomials such as "a+b+{2}a".

### static_assert
It is used more extensively to forbid meaningless calls, such as
determinizing a law automaton.

## 2012-07-13
### Precompiled contexts
Several predefined contexts come with their own header (e.g.,
"ctx/char_b_lal"), and their own library (e.g., "libchar_b_lal").  This is
provide for char_{b,z,zmin}_{lal,law}.

### z_min renamed zmin
Consistently with Vaucanson 1.4.

## 2012-07-10
### transposition
Transposition on automaton is a read/write view: operations such as
`del_state`, `add_transition`, etc. on a transposed automaton actually modify
the wrapped automaton: `set_final` calls `set_initial` and so forth.

As an extreme example, the following snippet:

    using context_t = vcsn::ctx::char_b;
    using automaton_t = vcsn::mutable_automaton<context_t>;
    using tr_automaton_t = vcsn::details::transpose_automaton<automaton_t>;
    context_t ctx{{'a', 'b'}};
    auto ks = ctx.make_kratexpset();
    auto aut = vcsn::standard_of<tr_automaton_t>(ctx, ks.conv("a+a+a+a"));

applies the standard-of algorithm to a transposed `mutable_automaton`.  In
other words,

    aut.strip();

is the transposition of a standard automaton, except that it is a
`mutable_automaton`, not a `transpose_automaton<mutable_automaton>`.

## 2012-07-09
### transposition
The "transpose" operation is implemented on words, weights, kratexps, and
automata.  pprat provides support to transpose on kratexps (option -t):

    pprat -W zrr -t {{{2}ab}cd}abcd   =>   {{{2}ba}dc}dcba
    pprat -W zrr -t {ab}(abcd)*{cd}   =>   ({dc}(dcba)*{ba})

and on (standard) automata (option -T):

    $ pprat -A w -W br    -s '{ab}(\e+a+b({abc}c{bcd})*){cd}' > forward.dot
    $ pprat -A w -W br -T -s '{ab}(\e+a+b({abc}c{bcd})*){cd}' > transpose.dot
    $ diff -W80 -t  -y forward.dot transpose.dot
    digraph A {                            digraph A {
      rankdir=LR                             rankdir=LR
      node [shape=circle]                    node [shape=circle]
      {                                      {
        node [style=invis,shape=none,la        node [style=invis,shape=none,la
        I1                                     I1
                                        >      I2
                                        >      I4
        F1                                     F1
        F2                              <
        F4                              <
      }                                      }
      1 -> F1 [label="{(ab).(cd)}"]     |    I1 -> 1 [label="{(dc).(ba)}"]
      I1 -> 1                           |    1 -> F1
      2 -> F2 [label="{cd}"]            |    I2 -> 2 [label="{dc}"]
      1 -> 2 [label="{ab}a"]            |    2 -> 1 [label="{ba}a"]
      4 -> F4 [label="{cd}"]            |    I4 -> 4 [label="{dc}"]
      4 -> 4 [label="{(abc).(bcd)}c"]   |    4 -> 4 [label="{(dcb).(cba)}c"]
      1 -> 3 [label="{ab}b"]            |    3 -> 1 [label="{ba}b"]
      3 -> 4 [label="{(abc).(bcd)}c"]   |    4 -> 3 [label="{(dcb).(cba)}c"]
    }                                      }

## 2012-06-19
### aut_to_exp
An initial version of `aut_to_exp` is available.  The new pprat option -a
provides an access to this algorithm: apply `aut_to_exp` to the
`standard_of` an expression.

    pprat      -a 'a*'     => \e+(a.(a*))
    pprat      -a '(a+b)c' => (a.c)+(b.c)
    pprat -W z -a '{2}({3}a+{5}b){7}c{11}' => (({6}a.{7}c)+({10}b.{7}c)){11}

Currently, the only "heuristic" implemented eliminates the states in order.
There are probably possible improvements.

    pprat -a '(a+b)*' | wc -c => 265

### pprat: -a and -w are renamed -A and -W

### new factory: de Bruijn
Builds automata for (a+b)a(a+b)^n.

## 2012-06-18
### standard_of is part of vcsn::
It used to be in vcsn::rat::.

## 2012-06-14
### kratexpset/abstract_kratexpset
kratexpset, i.e. the object that provides operation on kratexps (with
specified Gen and Weight), used to derive from abstract_kratexp (which is
"opaque": it does not know the precise type that is used underneath).

Now, from abstract_kratexp we derive a concrete_abstract_kratexpset which
*aggregates* a kratexpset.  This means that kratexpset no longer derives
from a weakly-typed ancestor, and can provide simple and strongly-typed
routines.

### kratexpset/kratexp
For consistency with weightset/weight, genset/gen, kratexps (note the s) is
renamed as kratexpset and std::shared_ptr<const rat::node> as kratexp.

### lift
A new algorithm which creates, from an automaton, another one with the same
states and transitions, but the new automaton features only spontaneous
transitions, whose weights correspond to the labels (and weights) of the
initial one.

For instance:

    $ pprat -aw -wz  -sl '({2}\e+{3}a){4}'
    digraph A {
      rankdir=LR
      node [shape=circle]
      {
        node [style=invis,shape=none,label="",width=0,height=0]
        I1
        F1
        F2
      }
      1 -> F1 [label="{8}"]
      I1 -> 1
      2 -> F2 [label="{4}"]
      1 -> 2 [label="{3}a"]
    }

    digraph A {
      rankdir=LR
      node [shape=circle]
      {
        node [style=invis,shape=none,label="",width=0,height=0]
        I1
        F1
        F2
      }
      1 -> F1 [label="{{8}\\e}"]
      I1 -> 1
      2 -> F2 [label="{{4}\\e}"]
      1 -> 2 [label="{{3}a}\\e"]
    }

### RatExps: fix is_unit
is_unit simply checked that the expression was \e, but did not check that
weight itself was the unit.

### VCSN_DEBUG
This variable allows to force the display of weights.

## 2012-06-11
### Contexts aggregate shared pointers
Now contexts are mutable, and hold (shared) pointers to (immutable) gensets
and weightsets.  This way, we can alter contexts (e.g., the ladybird factory
can add the letters it needs in a new genset), yet there is good sharing,
and identity can still be used to distinguish, for instance, two gensets
defined equally.

It is also simpler to really expose them as pointers, so every
"weightset().mul", etc. must be rewritten as "weightset()->mul".

## 2012-06-08
### Contexts
The Kind parameter is now part of the context.  The same type of Kind is now
use for both RatExps and automata.  This results in many significant
simplifications.

For instance, again, the test case for product:

Before:

     using context_t = vcsn::ctx::char_z;
     context_t ctx { {'a', 'b', 'c'} };
     using automaton_t =
       vcsn::mutable_automaton<context_t, vcsn::labels_are_letters>;
     automaton_t aut1(ctx);

After:

    using context_t = vcsn::ctx::char_z;
    context_t ctx { {'a', 'b', 'c'} };
    using automaton_t = vcsn::mutable_automaton<context_t>;

Or the source of pprat:

Before:

    using atom_kind_t
      = typename Factory::kind_t;
    using label_kind_t
      = typename vcsn::label_kind<atom_kind_t>::type;
    using context_t =
      vcsn::ctx::context<typename Factory::genset_t,
                         typename Factory::weightset_t,
                         label_kind_t>;
    context_t ctx{factory.genset(), factory.weightset()};
    using automaton_t = vcsn::mutable_automaton<context_t>;
    auto aut = vcsn::rat::standard_of<automaton_t>(ctx, e);

After:

    using context_t = typename Factory::context_t;
    using automaton_t = vcsn::mutable_automaton<context_t>;
    auto aut = vcsn::rat::standard_of<automaton_t>(factory.context(), e);


## 2012-06-05
### Contexts
"Contexts" were introduced to factor two aspects that are required through
out the library: the GenSet type (i.e., the nature of the generators), and
the WeightSet type (i.e., the nature of the weights).  Not only do contexts
define these types, they must also be instantiated so that "run-time"
details be known: for instance the set of generators is dynamic (what are
the allowed letters), and on occasion the weightset also needs run-time
information (e.g., when RatExp are parameterized by RatExp, what is the
alphabet of the latter ones?).

As an example of the changes on the user side, consider the product
test-case.

Before:

     typedef vcsn::set_alphabet<vcsn::char_letters> alpha_t;
     typedef vcsn::mutable_automaton<alpha_t, vcsn::z,
                                    vcsn::labels_are_letters> automaton_t;
     vcsn::z z;
     alpha_t alpha{'a', 'b', 'c'};
     automaton_t aut1(alpha, z);

After:

     using context_t = vcsn::ctx::char_z;
     context_t ctx { {'a', 'b', 'c'} };
     using automaton_t =
       vcsn::mutable_automaton<context_t, vcsn::labels_are_letters>;
     automaton_t aut1(ctx);

## 2012-05-30
### RatExp: atoms are words
Expressions such as "(ab)(ab)" used to be equivalent to "(abab)" (a
single four-letter atom).  Now:

    pprat -aw '(ab)(ab)'   => (ab).(ab)
    pprat -aw 'abab'       => abab
    pprat -aw 'ab.ab'      => (ab).(ab)
    pprat -aw 'ab(ab)abc*' => (ab).(ab).(ab).(c*)

## 2012-05-28
### New algorithm: eval, evaluates a word over an (weighted) automaton
Defined in vcsn/algos/eval.hh as vcsn::eval.

### New algorithm: determinize, Boolean automaton determinization
Defined in vcsn/algos/determinize.hh as vcsn::determinize.

## 2012-05-25
### RatExp: changes in the display
Fixed the output of "atoms are words" expressions.  For instance "(ab)*"
used to be displayed as "ab*" (which is wrong as it is parsed as "a(b*)").
It is now properly displayed as "(ab)*".

### RatExp: slight changes in the grammar
A star is now valid after a weight:

    $ pprat -w z '{2}ab{3}*'
    ({2}(a.b){3})*

## 2012-05-11
### dotty: define initial/final states first
In order to improve readability, instead of

    digraph A {
      rankdir=LR
      node [shape=circle];
      F1 [style=invis,shape=none,label="",width=0,height=0]
      1 -> F1 [label="{a.a.((d.d)*)}"]
      3 -> 2 [label="{(d.d)*}b"]
      I1 [style=invis,shape=none,label="",width=0,height=0]
      I1 -> 1
      1 -> 2 [label="{a.a.((d.d)*)}b"]
      F3 [style=invis,shape=none,label="",width=0,height=0]
      3 -> F3 [label="{(d.d)*}"]
      2 -> 3 [label="b"]
    }

we now produce

    digraph A {
      rankdir=LR
      node [shape=circle]
      {
        node [style=invis,shape=none,label="",width=0,height=0]
        I1
        F1
        F3
      }
      1 -> F1 [label="{a.a.((d.d)*)}"]
      3 -> 2 [label="{(d.d)*}b"]
      I1 -> 1
      1 -> 2 [label="{a.a.((d.d)*)}b"]
      3 -> F3 [label="{(d.d)*}"]
      2 -> 3 [label="b"]
    }

## 2012-05-10
### RatExp: support for the kind of atoms
As a major overhaul, the rational expressions (vcsn::rat::node) are now
parameterized by Atom, which denotes the atom value.  The kratexps structure
is now parameterized by the Kind, from which it is deduced, from the GenSet
parameter, whether we should use word_t or letter_t atoms.

### pprat: an option -a
To provide user-access to these feature, pprat now supports an option -a,
which accepts "letters" or "words" as argument, with obvious meaning.  For
instance:

    pprat -a letters 'abc' => a.b.c
    pprat -a letters 'abc.abc' => a.b.c.a.b.c
    pprat -w br -al '{aa}bb{c}dd{a}' => ({a.a}(b.b).{c}(d.d)){a}

    pprat -a words 'abc' => abc
    pprat -a words 'abc.abc' => abc.abc
    pprat -w br -aw '{aa}bb{c}dd{a}' => ({aa}bb.{c}dd){a}

Of course, this also works with the "standard-of" option:

    $ pprat -w br -al '{aa}({dd}\e+bb)*'
    {a.a}(({d.d}\e+(b.b))*)
    $ pprat -s -w br -al '{aa}({dd}\e+bb)*'
    digraph A {
      rankdir=LR
      node [shape=circle];
      F1 [style=invis,shape=none,label="",width=0,height=0]
      1 -> F1 [label="{a.a.((d.d)*)}"]
      3 -> 2 [label="{(d.d)*}b"]
      I1 [style=invis,shape=none,label="",width=0,height=0]
      I1 -> 1
      1 -> 2 [label="{a.a.((d.d)*)}b"]
      F3 [style=invis,shape=none,label="",width=0,height=0]
      3 -> F3 [label="{(d.d)*}"]
      2 -> 3 [label="b"]
    }

versus:

    $ pprat  -w br -aw '{aa}({dd}\e+bb)*'
    {aa}(({dd}\e+bb)*)
    $ pprat -s -w br -aw '{aa}({dd}\e+bb)*'
    digraph A {
      rankdir=LR
      node [shape=circle];
      F1 [style=invis,shape=none,label="",width=0,height=0]
      1 -> F1 [label="{aa.(dd*)}"]
      2 -> 2 [label="{dd*}bb"]
      F2 [style=invis,shape=none,label="",width=0,height=0]
      2 -> F2 [label="{dd*}"]
      1 -> 2 [label="{aa.(dd*)}bb"]
      I1 [style=invis,shape=none,label="",width=0,height=0]
      I1 -> 1
    }


## 2012-05-07
### RatExp: in some case the weights could be lost
"Associativity" was applied too eagerly, which would result in loss of some
weights.  E.g. {a}bb{c}dd resulted in b.b.d.d, not it evaluates to
{a}(b.b).{c}(d.d).

## 2012-05-03
### RatExp: improved pretty-printing
The outermost pair of parentheses is removed if useless.  For instance:

     (a.b) => a.b
     (a+b+c) => a+b+c
     (a*) => a*

But in the following examples they are kept.

     {3}(a.b){4}
     {3}(a*)
     (a+b){4}

### standard-of: star is fixed
Standard-of seems to be correct.

## 2012-04-25
### Expressions overhaul
They are immutable: we no longer make side effects on expressions.
They are shared_ptr, no longer plain pointers.  They *can* be used
like the other values, by value.

## 2012-04-19
### standard-of is fully implemented
Support for star was implemented, and checked for B and Z.  For
implementation reasons, one cannot yet use rational expressions as
weights.

## 2012-04-18
### mutable_automata::mul_weight

### RatExp::head and tail

### dotty
In order to improve the readability of its output, it no longer "defines"
the reachable states.  See the following diff:

     digraph A {
       rankdir=LR
       node [shape=circle];
    -  1
    -  2
    -  3
    -  4
       I1 [style=invis,shape=none,label="",width=0,height=0]
       I1 -> 1 [label="{6}"]
       F1 [style=invis,shape=none,label="",width=0,height=0]
       1 -> F1
       1 -> 2 [label="a"]
       1 -> 3 [label="a"]
       2 -> 4 [label="{3}b"]
       F4 [style=invis,shape=none,label="",width=0,height=0]
       4 -> F4
       4 -> 3 [label="a"]
     }


### standard-of: many fixes in the handling of the weights
An expression such as `{12}\e` used to leave the weight in the initial
transition; it is now on the final transition.  More generally the initial
transition always has unit as weight.

The product and sum of expressions now handle the left and right weights.

Accepting initial states in expressions such as "\e+a" are no longer lost.

## 2012-04-11
### Many renamings
    alphabet_t/alphabet() -> genset_t/genset(), etc.
    factory -> abstract_kratexp.
    factory_ -> kratexp.
    initials() -> initial_transitions(), etc.
    invalid_state -> null_state, etc.
    nb_state() -> num_states(), etc.
    polynomial -> polynomials, etc.

## 2012-04-09
### product

    An implementation of the product of two automata is available.

## 2012-04-07
### z_min

    An example of tropical semiring, to test show_unit().

## 2012-04-05
### char_letters::special()

    This method return a special, reserved character, that is used to
    label initial and final transitions.  This character is not part of
    the alphabet and is never output.

## 2012-04-04
### standard-of
Initial work on "+".

### mutable_automata are implemented using a pre() and post() states

    What is missing is the correct '$' letter on the initial and final
    transitions.  The current value is the default value for label_t.

    The previous interface has been preserved (but maybe we should
    clean it) except for one change:

      initial() and final() have been changed to return a pseudo
      container of transitions.  These transitions give us both
      the weight and the initial/final state.

    The following methods are new:

      pre(), post()         returns the pre-initial and post-final state.
      all_states()          returns all states, including pre() and post()
      all_transitions()     returns all transitions, including initial and
                              final transitions
      all_entries()         likewise for entries
      all_out(s), all_in(s) likewise for outgoing and ingoing transitions


    The methods get_initial_weight(s) and get_final_weight(s) are slower
    now, because they need to locate the corresponding initial/final
    transition.  For the same reason, is_initial(), is_final() are also
    slower.

## 2012-04-03
### standard-of
Initial version of standard-of is implemented.  Can be tested with
pprat's new option -s:

    $ pprat -wz -s '{123}a'
    digraph A {
      rankdir=LR
      node [shape=circle];
      1
      I1 [style=invis,shape=none,label="",width=0,height=0]
      I1 -> 1
      2
      F2 [style=invis,shape=none,label="",width=0,height=0]
      2 -> F2
      1 -> 2 [label="{123}a"]
    }

### mutable_automaton uses unsigned for state_t and transition_t.

This allows to store states in a std::vector<stored_state_t>.
Likewise for transitions.  Erased elements are marked (so they are
skipped over during iteration), and added to a free store to be reused
later.

### mutable_automaton does not store any weight when WeightSet == b.

### mutable_automaton has a read-only entry interface

    entries() is a pseudo container that filters transitions()
      to see each (src,dst) pair at most once.

    entry_at(src, dst) and entry_at(t) return a polynomial describing
      the entry between (src, dst) or (src_of(t), dst_of(t)).

    entryset() returns the WeightSet that can be used to manipulate
      these polynomials.

### make check-rat, make check-unit

There is a check target for each subdirectory of tests/.

## 2012-04-02
### Alphabets are checked
pprat is hard-coded to use a, b, c, d for all the alphabets (including for
inner rational expressions):

    $ pprat -w b -e 'y'
    1.1: invalid word: y: invalid letter: y

### Weights are checked
As follows:

    $ pprat -w b -e '{12}a'
    1.1-5: invalid Boolean: 12

Unfortunately the locations are bad currently for complex weights:

    $ pprat -w zr -e '{x}a'
    1.1-4: 1.1: invalid word: x: invalid letter: x

To be fixed.

## 2012-03-30
### G++ 4.7 is required
We use constructs that are not supported by 4.6 (e.g., constructor
delegation).

### zrr
As a demonstration that rational expressions can be weights of rational
expressions, pprat supports '-w zrr' (Rat<Rat<Z>>):

     $ pprat -w zr -e '{{{2}{3}a}u}x{{{4}{5}\e}\e}'
     {{{120}a}u}x

<!--

LocalWords:  TAF LAL automata vcsn EOF lal determinize ratexpset gv xy mult vis
LocalWords:  rankdir invis labelset weightset circo taf dyn thompson lan Ee fx
LocalWords:  RatExp EFSM OpenFST efsm efstcompile fstdeterminize abc eps dl len
LocalWords:  efstdecompile coaccessible acyclic pre fsm EOFSM isymbols txt rmin
LocalWords:  fstcompile acceptor FAdo fado nfa readFromFile dfa toDFA wxyz nmin
LocalWords:  Vaucanson evena oddb zmin minab minblocka slowgrow aabbba aut trie
LocalWords:  stdin Brzozowski's divkbaseb nul bb WeightSet aaabbbb aaab TAK scc
LocalWords:  adhoc fstinfo LAU WeightSets ba num typename const aa Graphviz ANR
LocalWords:  Refactoring initializations tikz LaTeX LabelSet nullableset Af Wl
LocalWords:  nullable Determinization de bruijn Onull sys API renamings xyz GCD
LocalWords:  LetterSet UnitSet WordSet letterset unitset wordset postfix Ef qmp
LocalWords:  FSM's wc parameterized lao br zr lau pprat Wz Lw ratexp xml cd GMP
LocalWords:  genset RatExps krat kratexp kratexpset lae vname precompiled hehe
LocalWords:  templated polynomialset del abcd Metadata sed weightsets laz abdc
LocalWords:  xyw determinizing ctx libchar conv kratexps zrr dcba bcd dcb daut
LocalWords:  cba wz sl gensets mul GenSet typedef abab eval determinization Luu
LocalWords:  al Associativity ptr nb semiring ingoing pprat's src dst utf multi
LocalWords:  entryset subdirectory ispell american hadamard realtime OpenFST's
LocalWords:  tuplesets Optimizations variadic cominimization runtime cotrie zA
LocalWords:  letterized zpc letterize optimizations codeterministic timeit Lutz
LocalWords:  unweighted multitape expressionset Vaucansa Sylvain Sakarovitch xz
LocalWords:  Alexandre Duret Akim Demaille EPITA resyntaxed labelsets filename
LocalWords:  nullabled cominimize costandard codeterminize TikZ determinizable
LocalWords:  determinized multiprecision instantiations Szmidt Pietri Canh tex
LocalWords:  Luca Saiu Tolmer Ratexpsets ratexps commutativity distributivity
LocalWords:  seriesset algo boolean Schützenberger erebus DNDEBUG subword ijk
LocalWords:  superset Magnus brzozowski TikZ's LaTeX's IPython isomorphism yx
LocalWords:  Polynomialsets RuntimeError insplit vaucanson url config bool GCC
LocalWords:  RatExpSet ratexp's duplications tuples blazingly nullablesets bxy
LocalWords:  tuple bc ky dfg Luca's tafkit MacPorts Clément Démoulins distro
LocalWords:  Vagrantfile VirtualBox mkdir wget VM cb Shortlex kE Ek RatE moore
LocalWords:  tmp darwin inlined texdoc Moore's uv lhs rhs semirings aabb aab
LocalWords:  abb MacBook subexpressions monomials Antimirov aaaa monomial aaa
LocalWords:  Automake Makefile versa Boilod Barray Sébastien Piat Yann APIs
LocalWords:  Copigny levenshtein delgado hopcropft oneset sname disjunction
LocalWords:  expressionsets involutivity hopcroft starrable Gillard Boillod
LocalWords:  Thibaud Michaud executables ldiv ldivide lweight rdiv rdivide
LocalWords:  rweight bd quotkbaseb abcac SVG demangle gdb Vcsn's boxart baz
LocalWords:  dijkstra acca FAdo's Sarasvati Moutoucomarapoulé Younes param
LocalWords:  Khoudli unabbreviated cerny lgcd ARGS qux quuux wrt Vcsners ef
LocalWords:  SMS libyaml cpp Sms François Yvon ipynb ipython Qunused Harald
LocalWords:  cxxflags Eppstein eppstein Soudière Schilly SageMathCloud prog
LocalWords:  abcdefghijklmnopqrstuvwxyz macOS rpath Doxygen doxygen Tupling
LocalWords:  tupleset dir dbf CoCalc ints xfe bzip sms dcae

Local Variables:
coding: utf-8
ispell-dictionary: "american"
fill-column: 76
mode: markdown
End:

-->
