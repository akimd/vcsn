#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <vcsn/algos/fwd.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/types.hh>
#include <vcsn/misc/export.hh> // LIBVCSN_API

namespace vcsn
{
  namespace dyn LIBVCSN_API
  {
    /// The accessible subautomaton of \a aut.
    automaton accessible(const automaton& aut);

    /// An ambiguous word, or raise if there is none.
    word ambiguous_word(const automaton& aut);

    /// Whether are the same automaton.
    bool are_equal(const automaton& lhs, const automaton& rhs);

    /// Whether are the same expression.
    bool are_equal(const expression& lhs, const expression& rhs);

    /// Whether compute the same series.
    /// \pre The labelsets of lhs and rhs are free.
    /// \pre lhs and rhs are Boolean, or on Z, or on a field.
    bool are_equivalent(const automaton& lhs, const automaton& rhs);

    /// Whether denote the same series.
    /// \pre The labelsets of lhs and rhs are free.
    /// \pre lhs and rhs are Boolean, or on Z, or on a field.
    bool are_equivalent(const expression& lhs, const expression& rhs);

    /// Whether there exists an isomorphism between the states of \a
    /// lhs and those of \a rhs.
    bool are_isomorphic(const automaton& lhs, const automaton& rhs);

    /// Produce a Černý automaton of \a num_states states.
    automaton cerny(const context& ctx, unsigned num_states);

    /// The coaccessible subautomaton of \a aut.
    automaton coaccessible(const automaton& aut);

    /// The codeterminized automaton.
    ///
    /// \param aut        the automaton to codeterminize
    /// \param algo
    ///   - "boolean"     use efficient bitsets
    ///   - "weighted"    accept non Boolean automata (might not terminate)
    ///   - "auto"        "boolean" if the automaton is Boolean,
    ///                   "weighted" otherwise.
    /// \pre  the labelset of \a aut must be free.
    automaton codeterminize(const automaton& aut,
                            const std::string& algo = "auto");

    /// The cominimized automaton.
    ///
    /// \param aut       the automaton to cominimize
    /// \param algo      the specific algorithm to use
    /// \pre  \a aut must be LAL.
    /// \pre  \a aut must be deterministic.
    automaton cominimize(const automaton& aut,
                         const std::string& algo = "auto");

    /// Three-way comparison between automata.
    /// \returns negative if `lhs < rhs`
    ///          null     if `lhs = rhs`
    ///          positive if `lhs > rhs`
    int compare(const automaton& lhs, const automaton& rhs);

    /// Three-way comparison between expressions.
    /// \returns negative if `lhs < rhs`
    ///          null     if `lhs = rhs`
    ///          positive if `lhs > rhs`
    int compare(const expression& lhs, const expression& rhs);

    /// The complement of \a aut.
    ///
    /// \pre aut is lal
    /// \pre aut is Boolean
    /// \pre aut is deterministic
    /// \pre aut is complete
    automaton complement(const automaton& aut);

    /// Complement this expansion.
    expansion complement(const expansion& x);

    /// Add the complement operator to \a r.
    expression complement(const expression& r);

    /// A completed copy of \a aut.
    /// \pre aut is lal
    automaton complete(const automaton& aut);

    /// The automaton of a strongly connected component.
    ///
    /// \param aut    the input automaton.
    /// \param num    the component number.
    automaton component(const automaton& aut, unsigned num);

    /// The composition of transducers \a lhs and \a rhs.
    ///
    /// \param lhs   the left transducer
    /// \param rhs   the right transducer
    /// \param lazy  whether to perform the computations on demand.
    automaton compose(const automaton& lhs, const automaton& rhs,
                      bool lazy = false);

    /// The composition of two expansions.
    expansion compose(const expansion& lhs, const expansion& rhs);

    /// The composition of two expressions.
    expression compose(const expression& lhs, const expression& rhs);

    /// The composition of two polynomials.
    polynomial compose(const polynomial& lhs, const polynomial& rhs);

    /// Concatenate two expressions.
    ///
    /// Do not use this routine, see multiply instead.
    expression concatenate(const expression& lhs, const expression& rhs);

    /// The condensation of \a aut such that each state is a strongly
    /// connected component.
    automaton condense(const automaton& aut);

    /// The conjunction (aka synchronized product) of automata.
    /// Performs the meet of the contexts.
    ///
    /// \param lhs   the left automaton
    /// \param rhs   the right automaton
    /// \param lazy  whether to perform the computations on demand.
    automaton conjunction(const automaton& lhs, const automaton& rhs,
                          bool lazy = false);

    /// The conjunction (aka synchronized product) of automata.
    /// Performs the meet of the contexts.
    ///
    /// \param as    the automata
    /// \param lazy  whether to perform the computations on demand.
    automaton conjunction(const std::vector<automaton>& as,
                          bool lazy = false);

    /// Repeated conjunction of \a aut with itself.
    automaton conjunction(const automaton& aut, unsigned n);

    /// The Hadamard product of expansions \a lhs and \a rhs.
    expansion conjunction(const expansion& lhs, const expansion& rhs);

    /// The Hadamard product of expressions \a lhs and \a rhs.
    expression conjunction(const expression& lhs, const expression& rhs);

    /// The Hadamard product of polynomials \a lhs and \a rhs.
    polynomial conjunction(const polynomial& lhs, const polynomial& rhs);

    /// The conjugate of \a aut
    automaton conjugate(const automaton& aut);

    /// Access a configuration value
    std::string configuration(const std::string& key);

    /// The weight associated to the empty word in \a e.
    weight constant_term(const expression& e);

    /// The context of this automaton.
    context context_of(const automaton& a);

    /// The context of this expansion.
    context context_of(const expansion& r);

    /// The context of this expression.
    context context_of(const expression& r);

    /// The context of this polynomial.
    context context_of(const polynomial& r);

    /// A copy of \a aut.
    automaton copy(const automaton& aut);

    /// A copy of \a aut converted to context \a ctx.
    automaton copy(const automaton& aut, const context& ctx);

    /// Copy of \a exp, but built with \a ctx.
    expression copy(const expression& exp,
                    const context& ctx, identities ids);

    /// A co-standardized \a a.
    automaton costandard(const automaton& a);

    /// A reversed trie-like automaton (multiple initial states,
    /// single final state) automaton to accept \a p.
    ///
    /// \param p    the polynomial that describes the (finite) series
    automaton cotrie(const polynomial& p);

    /// A reversed trie-like automaton (multiple initial states,
    /// single final state) automaton to accept the series contained
    /// in \a is.
    ///
    /// \param ctx     the (word-based) context used to read the words.
    /// \param is      the input stream.
    /// \param format  the format of the file: "words" or "monomials".
    automaton cotrie(const context& ctx, std::istream& is,
                     const std::string& format = "default");

    /// A simple NFA for (a+b)*a(a+b)^n.
    automaton de_bruijn(const context& ctx, unsigned n);

    /// The automaton with the delay of each state.
    automaton delay_automaton(const automaton& a);

    /// Derive \a exp with respect to \a s.
    ///
    /// \param exp       the input expression
    /// \param lbl       the label used for derivation
    /// \param breaking  whether to split the result
    /// \pre  \a exp must be LAL.
    polynomial derivation(const expression& exp, const label& lbl,
                          bool breaking = false);

    /// The derived-term automaton of \a exp.
    ///
    /// \param exp    the input expression
    /// \param algo   how the derived terms are computed:
    ///    - "auto"                  alias for "expansion"
    ///    - "derivation"            compute by derivation
    ///    - "breaking_derivation"   compute by breaking derivation
    ///    - "expansion"             compute by expansion
    ///    - "breaking_expansion"    compute by breaking expansion
    automaton derived_term(const expression& exp,
                           const std::string& algo = "auto");

    /// The determinized automaton.
    ///
    /// \param aut        the automaton to determinize
    /// \param algo
    ///     - "boolean"     use efficient bitsets
    ///     - "weighted"    accept non Boolean automata (might not terminate)
    ///     - "auto"        "boolean" if the automaton is Boolean,
    ///                     "weighted" otherwise.
    /// \pre  the labelset of \a aut must be free.
    automaton determinize(const automaton& aut,
                          const std::string& algo = "auto");

    /// An automaton whose behavior is that of \a lhs on words not
    /// accepted by \a rhs.
    ///
    /// \param lhs   a LAL automaton
    /// \param rhs   a LAL Boolean automaton
    /// \pre \a rhs is Boolean.
    automaton difference(const automaton& lhs, const automaton& rhs);

    /// Words accepted by \a lhs, but not by \a rhs.
    expression difference(const expression& lhs, const expression& rhs);

    /// An automaton which accepts a word n representing a number in
    /// base b iff k|n.
    automaton divkbaseb(const context& ctx, unsigned divisor, unsigned base);

    /// The double_ring automaton with \a n states and \a f the list of finals.
    automaton double_ring(const context& ctx, unsigned n,
                          const std::vector<unsigned>& f);

    /// The LAO automaton \a aut with state \a s removed.
    automaton eliminate_state(const automaton& aut,
                              int s = -1);

    /// Evaluate \a l on \a aut.
    weight evaluate(const automaton& aut, const word& l);

    /// Evaluate \a p on \a aut.
    weight evaluate(const automaton& aut, const polynomial& p);

    /// Distribute product over addition recursively under the starred
    /// subexpressions and group the equal monomials.
    expression expand(const expression& e);

    /// The expression for `\\e`.
    expression expression_one(const context& c, identities ids);

    /// The expression for `\\z`.
    expression expression_zero(const context& c, identities ids);

    /// Create a factor automaton from \a aut.
    automaton factor(const automaton& aut);

    /// The subautomaton based on \a aut, with only states in \a ss visible.
    automaton filter(const automaton& aut, const std::vector<unsigned>& ss);

    /// Focus on a specific tape of a tupleset automaton.
    automaton focus(const automaton& aut, unsigned tape);

    /// Whether the automaton has the twins property.
    bool has_twins_property(const automaton& aut);

    /// Whether the automaton has a cycle with lightening weight.
    bool has_lightening_cycle(const automaton& aut);

    /// Whether the transducer has bounded lag.
    bool has_bounded_lag(const automaton& aut);

    /// The identities of expression \a exp.
    identities identities_of(const expression& exp);

    /// The inductive translation of \a e in an automaton.
    ///
    /// \param e     the expressions
    /// \param algo  how to compute the result
    ///   - "standard"    build a standard automaton.
    ///   - "general"     no requirement on the result
    ///   - "auto"        same as "standard"
    automaton inductive(const expression& e, const std::string& algo = "auto");

    /// The infiltration of automata \a lhs and \a rhs.
    /// Performs the join of their types.
    automaton infiltrate(const automaton& lhs, const automaton& rhs);

    /// The infiltration product of automata.
    /// Performs the join of their types.
    automaton infiltrate(const std::vector<automaton>& as);

    /// The infiltration product of expressions \a lhs and \a rhs.
    /// Performs the join of their type.
    expression infiltrate(const expression& lhs, const expression& rhs);

    /// Output various facts about an automaton.
    ///
    /// \param aut  the automaton under study
    /// \param out  the output stream where to print the info
    /// \param details
    ///    levels of details from 1 (fewer), to 3 (more).
    std::ostream& info(const automaton& aut, std::ostream& out = std::cout,
                       unsigned details = 2);

    /// Output various facts about \a exp.
    std::ostream& info(const expression& exp, std::ostream& out = std::cout);

    /// Split automaton on the incoming transition.
    ///
    /// In an insplit automaton, each state has either only
    /// spontaneous incoming transitions or only proper incoming
    /// transitions.
    ///
    /// \param aut   automaton to insplit
    /// \param lazy  whether to perform the computations on demand.
    ///
    /// \pre aut is lal or lan.
    automaton insplit(const automaton& aut, bool lazy = false);

    /// Whether \a aut is accessible.
    bool is_accessible(const automaton& aut);

    /// Whether \a aut is ambiguous.
    ///
    /// \pre \a aut is LAL.
    bool is_ambiguous(const automaton& aut);

    /// Whether \a aut is coaccessible.
    bool is_coaccessible(const automaton& aut);

    /// Whether \a aut is codeterministic.
    ///
    /// \pre \a aut is LAL.
    bool is_codeterministic(const automaton& aut);

    /// Whether \a aut is complete.
    ///
    /// \pre \a aut is LAL.
    bool is_complete(const automaton& aut);

    /// Whether is costandard (unique final state, with weight one, no
    /// outcoming transition).
    bool is_costandard(const automaton& aut);

    /// Whether the automaton is cycle-ambiguous.
    bool is_cycle_ambiguous(const automaton& aut);

    /// Whether \a aut is deterministic.
    /// \pre \a aut is LAL.
    bool is_deterministic(const automaton& aut);

    /// Whether has no state.
    bool is_empty(const automaton& aut);

    /// Whether has no cycle of spontaneous transitions.
    bool is_eps_acyclic(const automaton& aut);

    /// Whether \a aut is functional.
    ///
    /// \pre \a aut is a transducer.
    bool is_functional(const automaton& aut);

    /// Whether \a aut is letterized.
    bool is_letterized(const automaton& aut);

    /// Whether \a aut realizes a partial identity.
    ///
    /// \pre \a aut is a transducer.
    bool is_partial_identity(const automaton& aut);

    /// Whether is normalized (in the Thompson sense), i.e., standard
    /// and co-standard.
    bool is_normalized(const automaton& aut);

    /// Whether the outgoing transitions of each state have increasing labels.
    bool is_out_sorted(const automaton& aut);

    /// Whether has no spontaneous transition.
    bool is_proper(const automaton& aut);

    /// Whether has no spontaneous transition, and is letterized.
    bool is_realtime(const automaton& aut);

    /// Whether is standard (unique initial state, with weight one, no
    /// incoming transition).
    bool is_standard(const automaton& aut);

    /// Whether is synchronized.
    bool is_synchronized(const automaton& aut);

    /// Whether the word synchronizes aut.
    bool is_synchronized_by(const automaton& aut, const word& word);

    /// Whether is synchronizing.
    bool is_synchronizing(const automaton& aut);

    /// Whether has no useless state.
    bool is_trim(const automaton& aut);

    /// Whether has no useful state.
    bool is_useless(const automaton& aut);

    /// Whether automaton is valid (epsilon-cycles converge).
    bool is_valid(const automaton& e);

    /// Whether rational expression is valid (all the starred
    /// sub-expressions are starrable).
    bool is_valid(const expression& e);

    /// The join between two contexts, i.e., their lowest common supertype.
    context join(const context& lhs, const context& rhs);

    /// The ladybird automaton with \a n states.
    automaton ladybird(const context& ctx, unsigned n);

    /// Left-division of two automata (lhs \ rhs).
    automaton ldivide(const automaton& lhs, const automaton& rhs);

    /// Left-division of two expansions (lhs \ rhs).
    expansion ldivide(const expansion& lhs, const expansion& rhs);

    /// Left-division of two expressions (lhs \ rhs).
    expression ldivide(const expression& lhs, const expression& rhs);

    /// Left-division of two labels (lhs \ rhs).
    label ldivide(const label& lhs, const label& rhs);

    /// Left-division of two polynomials (lhs \ rhs).
    polynomial ldivide(const polynomial& lhs, const polynomial& rhs);

    /// Left-division of two weights (lhs \ rhs).
    weight ldivide(const weight& lhs, const weight& rhs);

    /// Left greatest common divisor of two polynomials (lhs \ rhs).
    polynomial lgcd(const polynomial& lhs, const polynomial& rhs);

    /// The left-multiplication of an automaton with \a w as weight.
    ///
    /// \param w     the weight to left-multiply
    /// \param aut   the input automaton.
    /// \param algo  how to compute the result.
    ///   - "standard"    \a aut is standard,
    ///                   build a standard automaton.
    ///   - "general"     no requirement on \a aut,
    ///                   but add spontaneous transitions.
    ///   - "auto"        "standard" if \a aut is standard,
    ///                   "general" otherwise.
    automaton lweight(const weight& w, const automaton& aut,
                        const std::string& algo = "auto");

    /// The left-multiplication of an expansion with \a w as weight.
    expansion lweight(const weight& w, const expansion& exp);

    /// The left-multiplication of an expression with \a w as weight.
    expression lweight(const weight& w, const expression& exp);

    /// The left-multiplication of a polynomial with \a w as weight.
    polynomial lweight(const weight& w, const polynomial& p);

    /// Ordering between automata.
    bool less_than(const automaton& lhs, const automaton& rhs);

    /// Ordering between expressions.
    bool less_than(const expression& lhs, const expression& rhs);

    /// The automaton with letter transitions instead of words.
    automaton letterize(const automaton& aut);

    /// The transducer representing the Levenshtein distance.
    automaton levenshtein(const context& ls);

    /// Lift some tapes of the transducer, or turn an automaton into a
    /// spontaneous automaton.
    ///
    /// Each `<k>l` transition is mapped to a `<<k>l>\\e` transition.
    ///
    /// \param aut    the input automaton
    /// \param tapes  the tapes to lift to weights
    /// \param ids    the identities of the expression
    automaton lift(const automaton& aut,
                   const std::vector<unsigned>& tapes = {},
                   identities ids = {});

    /// The lifted LAO rational expression from \a e.
    expression lift(const expression& e);

    /// The approximated behavior of an automaton with smallest weights.
    ///
    /// \param aut   the automaton whose behavior to approximate
    /// \param num   number of words looked for.
    /// \param algo  the specific algorithm to use
    polynomial lightest(const automaton& aut, unsigned num = 1,
                        const std::string& algo = "auto");

    /// The shortest path automaton using the given algorithm.
    automaton lightest_automaton(const automaton& aut,
                                 unsigned num = 1,
                                 const std::string& algo = "auto");

    /// Build a context from its name.
    context make_context(const std::string& name);

    /// Build an automatonset from its context.
    automaton_editor* make_automaton_editor(const context& ctx);

    /// The context for words.
    context make_word_context(const context& ctx);

    /// Multiply (concatenate) two automata.
    ///
    /// \param lhs   an automaton.
    /// \param rhs   another one.
    /// \param algo  how to compute the result
    ///   - "standard"    both \a lhs and \a rhs are standard,
    ///                   build a standard automaton.
    ///   - "general"     no requirement on \a lhs and and \a rhs,
    ///                   but add spontaneous transitions.
    ///   - "auto"        "standard" if both automata are standard,
    ///                   "general" otherwise.
    automaton multiply(const automaton& lhs, const automaton& rhs,
                       const std::string& algo = "auto");

    /// Repeated multiplication (concatenation) of an automaton with itself.
    ///
    /// \param aut  the automaton.
    /// \param min  the minimum number.  If -1, denotes 0.
    /// \param max  the maximum number.
    ///             If -1, denotes infinity, using star.
    ///             If -2, denotes the same value as min.
    /// \param algo  how to compute the result
    ///   - "standard"    \a aut is standard,
    ///                   build a standard automaton.
    ///   - "general"     no requirement on \a aut,
    ///                   but add spontaneous transitions.
    ///   - "auto"        "standard" if \a aut is standard,
    ///                   "general" otherwise.
    automaton multiply(const automaton& aut, int min, int max = -2,
                       const std::string& algo = "auto");

    /// Multiply (concatenate) two expressions.
    expression multiply(const expression& lhs, const expression& rhs);

    /// Repeated multiplication (concatenation) of an expression with itself.
    ///
    /// \param e    the expression.
    /// \param min  the minimum number.  If -1, denotes 0.
    /// \param max  the maximum number.
    ///             If -1, denotes infinity, using star.
    ///             If -2, denotes the same value as min.
    expression multiply(const expression& e, int min, int max = -2);

    /// Multiply (concatenate) two labels.
    label multiply(const label& lhs, const label& rhs);

    /// Repeated multiplication of a label with itself.
    ///
    /// \param l    the label.
    /// \param num  the exponent.
    label multiply(const label& l, int num);

    /// Multiply two polynomials.
    polynomial multiply(const polynomial& lhs, const polynomial& rhs);

    /// Multiply two weights.
    weight multiply(const weight& lhs, const weight& rhs);

    /// Repeated multiplication of a weight with itself.
    ///
    /// \param e    the weight.
    /// \param min  the minimum number.  If -1, denotes 0.
    /// \param max  the maximum number.
    ///             If -1, denotes infinity, using star.
    ///             If -2, denotes the same value as min.
    weight multiply(const weight& e, int min, int max = -2);

    /// The minimized automaton.
    ///
    /// \param aut       the automaton to minimize
    /// \param algo      the specific algorithm to use
    /// \pre  \a aut must be LAL.
    /// \pre  \a aut must be deterministic.
    automaton minimize(const automaton& aut,
                       const std::string& algo = "auto");

    /// Normalize automaton \a aut.
    automaton normalize(const automaton& aut);

    /// The number of strongly connected components.
    std::size_t num_components(const automaton& aut);

    /// The number of input tapes.
    /// \returns 0 if the context is not tupleset.
    std::size_t num_tapes(const context& ctx);

    /// Build the pair automaton of the given automaton
    automaton pair(const automaton& aut, bool keep_initials = false);

    /// Create a partial identity transducer from \a aut.
    automaton partial_identity(const automaton& aut);

    /// Create a partial-identity two-tape expression from \a e.
    expression partial_identity(const expression& e);

    /// Create a prefix automaton from \a aut.
    automaton prefix(const automaton& aut);

    /// Print automaton \a a on \a out using format \a format.
    ///
    /// \param aut  the automaton to format
    /// \param out  the output stream where to print the dot file
    /// \param format  the output format.
    ///    - "default"  Same as "dot".
    ///    - "dot"      GraphViz' Dot format.
    ///    - "dot,utf8" Likewise, but benefit from UTF-8
    ///    - "dot,latex" Likewise, but generate LaTeX escapes for
    ///                 use with dot2tex.
    ///    - "efsm"     Extended FSM.
    ///    - "fado"     FAdo format.
    ///    - "grail"    Grail format.
    ///    - "tikz"     LaTeX's TikZ format.
    std::ostream& print(const automaton& aut, std::ostream& out = std::cout,
                        const std::string& format = "default");

    /// Print context \a c on \a out using format \a format.
    std::ostream& print(const context& c, std::ostream& out = std::cout,
                        const std::string& format = "default");

    /// Print expansion \a e on \a out using format \a format.
    std::ostream& print(const expansion& e, std::ostream& out = std::cout,
                        const std::string& format = "default");

    /// Print expression \a e on \a out using format \a format.
    std::ostream& print(const expression& e, std::ostream& out = std::cout,
                        const std::string& format = "default");

    /// Print label \a l on \a out using format \a format.
    std::ostream& print(const label& l, std::ostream& out = std::cout,
                        const std::string& format = "default");

    /// Print polynomial \a p on \a out using format \a format.
    std::ostream& print(const polynomial& p, std::ostream& out = std::cout,
                        const std::string& format = "default");

    /// Print weight \a w on \a out using format \a format.
    std::ostream& print(const weight& w, std::ostream& out = std::cout,
                        const std::string& format = "default");

    /// A transducer that computes the quotient of the division
    /// of a word n by k in base b iff k|n.
    automaton quotkbaseb(const context& ctx, unsigned divisor, unsigned base);

    /// Specify the output format for \a out.
    void set_format(std::ostream& out, const std::string& format);

    /// Get the output format for \a out.
    std::string get_format(std::ostream& out);

    /// Keep a single tape from a multiple-tape automaton.
    automaton project(const automaton& aut, unsigned tape);

    /// Keep a single tape from a multiple-tape context.
    context project(const context& ctx, unsigned tape);

    /// Keep a single tape from a multiple-tape expansion.
    expansion project(const expansion& xpn, unsigned tape);

    /// Keep a single tape from a multiple-tape expression.
    expression project(const expression& exp, unsigned tape);

    /// Keep a single tape from a multiple-tape polynomial.
    polynomial project(const polynomial& p, unsigned tape);

    /// Keep a single tape from a multiple-tape label.
    label project(const label& p, unsigned tape);

    /// An equivalent automaton without spontaneous transitions.
    ///
    /// \param aut    the automaton in which to remove them
    /// \param dir    the direction of the closure
    /// \param prune  whether to delete states that become inaccessible
    /// \param algo   how elimination is performed
    ///     - "auto"       same as "inplace"
    ///     - "default"    same as "inplace"
    ///     - "inplace"    eliminate in place
    ///     - "separate"   first separate the automaton into spontaneous and
    ///                    proper parts
    ///     - "distance"   compute all-pairs distances
    automaton proper(const automaton& aut,
                     direction dir = direction::backward,
                     bool prune = true, const std::string& algo = "auto");

    /// The weight pushing automaton of \a aut.
    automaton push_weights(const automaton& aut);

    /// A random automaton.
    ///
    /// \param ctx
    ///    The context of the result.
    /// \param num_states
    ///    The number of states wanted in the automata
    ///    (>0).  All states will be connected, and there will be no dead
    ///    state.  However, some might not be coaccessible.
    /// \param density
    ///    The density of the automata.  This is the probability
    ///    (between 0.0 and 1.0), to add a transition between two
    ///    states.  All states have at least one outgoing transition,
    ///    so \a d is considered only when adding the remaining transition.
    ///    A density of 1 means all states will be connected to each other.
    /// \param num_initial
    ///    The number of initial states wanted (0 <= num_initial <= num_states)
    /// \param num_final
    ///    The number of final states wanted (0 <= num_final <= num_states)
    /// \param max_labels
    ///    The maximum number of labels per transition.  Defaults to the
    ///    number of generators.
    /// \param loop_chance
    ///    The probability (between 0.0 and 1.0) for each state to have
    ///    a loop.
    /// \param weights
    ///    The specification string (following the format of random_weight) used
    ///    to generate weights on each transitions.
    automaton random_automaton(const context& ctx,
                               unsigned num_states,
                               float density = 0.1,
                               unsigned num_initial = 1,
                               unsigned num_final = 1,
                               boost::optional<unsigned> max_labels = {},
                               float loop_chance = 0.0,
                               const std::string& weights = "");

    /// A random deterministic automaton.
    ///
    /// \param ctx
    ///    The context of the result.
    /// \param num_states
    ///    The number of states wanted in the automata (>0).
    automaton random_automaton_deterministic(const context& ctx,
                                             unsigned num_states);

    /// A random expression.
    ///
    /// \param ctx
    ///    The context of the result.
    /// \param param
    ///    A specification of the wanted operators and their relative
    ///    probability.  May also contain the maximum number of
    ///    symbols of the expression with key `length`.  For instance:
    ///    "+=1,.=2,{T}=0.5,length=6".
    /// \param ids
    ///    The identities to use for the expression.
    expression
    random_expression(const context& ctx,
                      const std::string& param = "+, ., *=.2, w., .w, length=10",
                      identities ids = {});

    /// A random weight.
    ///
    /// \param ctx
    ///    The context of the result.
    /// \param param
    ///    A string containing paramaters for the generation such as the
    ///    min and max, or an element you want with specific probability.
    ///    For instance: "1=0.2, 10=0.3, min=0, max=20".
    weight random_weight(const context& ctx, const std::string& param);

    /// Right-division of two automata (lhs / rhs).
    automaton rdivide(const automaton& lhs, const automaton& rhs);

    /// Right-division of two expressions (lhs / rhs).
    expression rdivide(const expression& lhs, const expression& rhs);

    /// Right-division of two labels (lhs / rhs).
    label rdivide(const label& lhs, const label& rhs);

    /// Right-division of two weights (lhs / rhs).
    weight rdivide(const weight& lhs, const weight& rhs);

    /// Read an automaton from a stream.
    /// \param is      the input stream.
    /// \param format  its format.
    /// \param strip   whether to return a stripped automaton,
    ///                or a named automaton.
    automaton read_automaton(std::istream& is,
                             const std::string& format = "default",
                             bool strip = true);

    /// Read an expression from a stream.
    ///
    /// \param ctx     the context.
    /// \param ids     the identities to apply.
    /// \param is      the input stream.
    /// \param format  the expression's format.
    expression read_expression(const context& ctx, identities ids,
                               std::istream& is,
                               const std::string& format = "default");

    /// Read a label from a stream.
    ///
    /// \param ctx     the context.
    /// \param is      the input stream.
    /// \param format  the label's format.
    label read_label(const context& ctx, std::istream& is,
                     const std::string& format = "default");

    /// Read a polynomial from a stream
    ///
    /// \param ctx   its context, from which is built its polynomialset.
    /// \param is    the stream to parse.
    polynomial read_polynomial(const context& ctx, std::istream& is);

    /// Read a weight from a stream.
    ///
    /// \param ctx   the context.
    /// \param is    the input stream.
    weight read_weight(const context& ctx, std::istream& is);

    /// Create the realtime automaton (letterized and proper).
    automaton realtime(const automaton& aut);

    /// Reduce \a aut.
    automaton reduce(const automaton& aut);

    /// The right-mult automaton with \a w as weight.
    ///
    /// \param aut   the input automaton.
    /// \param w     the weight to right-multiply
    /// \param algo  how to compute the result.
    ///              In most cases, "standard" and "general" have
    ///              equal results.  The only difference in when
    ///              when \a w is null, in which case "general"
    ///              produces an empty automaton, while "standard"
    ///              produces an automaton with a single state, which
    ///              is initial (and, of course, no final states).
    ///   - "standard"    \a aut is standard,
    ///                   build a standard automaton.
    ///   - "general"     no requirement on \a aut,
    ///                   but add spontaneous transitions.
    ///   - "auto"        "standard" if \a aut is standard,
    ///                   "general" otherwise.
    automaton rweight(const automaton& aut, const weight& w,
                         const std::string& algo = "auto");

    /// The right-multiplication of an expansion with \a w as weight.
    expansion rweight(const expansion& aut, const weight& w);

    /// The right-multiplication of an expression with \a w as weight.
    expression rweight(const expression& exp, const weight& w);

    /// The right-multiplication of a polynomial with \a w as weight.
    polynomial rweight(const polynomial& p, const weight& w);

    /// Build the SCC automaton whose states are labeled with number
    /// of the strongly-connected component they belong to.
    ///
    /// \param aut    the input automaton.
    /// \param algo   the specific algorithm to use.
    automaton scc(const automaton& aut, const std::string& algo = "auto");

    /// The approximated behavior of an automaton.
    ///
    /// \param aut   the automaton whose behavior to approximate
    /// \param num   number of words looked for.
    /// \param len   maximum length of words looked for.
    polynomial shortest(const automaton& aut,
                        boost::optional<unsigned> num = {},
                        boost::optional<unsigned> len = {});

    /// The shuffle product of automata \a lhs and \a rhs.
    /// Performs the join of their type.
    automaton shuffle(const automaton& lhs, const automaton& rhs);

    /// The shuffle product of automata.
    /// Performs the join of their types.
    automaton shuffle(const std::vector<automaton>& as);

    /// The shuffle product of expressions \a lhs and \a rhs.
    /// Performs the join of their type.
    expression shuffle(const expression& lhs, const expression& rhs);

    /// A copy of \a a with normalized state numbers.
    automaton sort(const automaton& a);

    /// Break \a exp.
    polynomial split(const expression& exp);

    /// Break all the expressions in \a p.
    polynomial split(const polynomial& p);

    /// A standardized \a a.
    automaton standard(const automaton& a);

    /// The standard automaton of \a e.
    automaton standard(const expression& e);

    /// Star of an automaton.
    ///
    /// \param aut   the input automaton.
    /// \param algo  how to compute the result.
    ///   - "standard"    \a aut is standard,
    ///                   build a standard automaton.
    ///   - "general"     no requirement on \a aut,
    ///                   but add spontaneous transitions.
    ///   - "auto"        "standard" if \a aut is standard,
    ///                   "general" otherwise.
    automaton star(const automaton& aut, const std::string& algo = "auto");

    /// Star height of an expression.
    unsigned star_height(const expression& rs);

    /// A normalized form where star is applied only to proper
    /// expressions.
    ///
    /// \pre \a e is Boolean.
    expression star_normal_form(const expression& e);

    /// The automaton in \a a with its metadata layers removed.
    automaton strip(const automaton& a);

    /// Create a suffix automaton from \a aut.
    automaton suffix(const automaton& aut);

    /// Create a subword automaton from \a aut.
    ///
    /// For each non-spontaneous transition, add a spontaneous
    /// transition with same source, destination, and weight.
    automaton subword(const automaton& aut);

    /// Sum of two automata.
    ///
    /// \param lhs   an automaton.
    /// \param rhs   another one.
    /// \param algo  how to compute the result
    ///   - "standard"    both \a lhs and \a rhs are standard,
    ///                   build a standard automaton.
    ///   - "general"     no requirement on \a lhs and and \a rhs.
    ///   - "auto"        "standard" if both automata are standard,
    ///                   "general" otherwise.
    automaton add(const automaton& lhs, const automaton& rhs,
                  const std::string& algo = "auto");

    /// Sum of two expressions.
    expression add(const expression& lhs, const expression& rhs);

    /// Sum of two expansions.
    expansion add(const expansion& lhs, const expansion& rhs);

    /// Sum of two polynomials.
    polynomial add(const polynomial& lhs, const polynomial& rhs);

    /// Sum of two weights.
    weight add(const weight& lhs, const weight& rhs);

    /// Create a synchronized transducer from \a aut.
    automaton synchronize(const automaton& aut);

    /// A synchronizing word, or raise if there is none.
    word synchronizing_word(const automaton& aut,
                            const std::string& algo = "greedy");

    /// The Thompson automaton of \a e.
    automaton thompson(const expression& e);

    /// An automaton denoting the language of \a exp.
    /// \param exp   the expression defining the series.
    /// \param algo  the conversion algorithm.
    ///   - "auto": currently equivalent to "expansion", eventually should
    ///      mix "standard" for basic expressions (faster) and "expansion"
    ///      otherwise (more general).
    ///   - "derivation": use derivation-based expression.derived_term,
    ///      stripped.
    ///   - "derived_term": same as "expansion"
    ///   - "expansion": use expansion-based expression.derived_term, stripped.
    ///   - "inductive": use expression.inductive
    ///   - "standard": use expression.standard.
    ///   - "thompson": use expression.thompson.
    ///   - "zpc": use expression.zpc.
    ///   - "zpc,compact": use expression.zpc, "compact" version.
    automaton to_automaton(const expression& exp,
                           const std::string& algo = "auto");

    /// First order development of a \a exp.
    ///
    /// \param exp              the input expression
    expansion to_expansion(const expression& exp);

    /// An expression denoting the language of \a aut.
    ///
    /// \param aut   the input automaton
    /// \param ids   the identities to use for the expression
    /// \param algo  the state selection strategy
    expression to_expression(const automaton& aut,
                             identities ids = {},
                             const std::string& algo = "auto");

    /// An expression denoting the label of \a l.
    expression to_expression(const context& ctx, identities ids,
                             const label& l);

    /// An expression matching one letter in a letter class.
    ///
    /// \param ctx
    ///   The context to use.
    /// \param ids
    ///   The identities to use for the expression
    /// \param letters
    ///   The letter class as a set of ranges.
    /// \param accept
    ///   Whether to accept these characters ([abc]) as opposed
    ///   to refusing them ([^abc]).
    expression to_expression(const context& ctx, identities ids,
                             const letter_class_t& letters,
                             bool accept = true);

    /// Transpose \a aut.
    automaton transpose(automaton& aut);

    /// Transpose \a e.
    expression transpose(const expression& e);

    /// Add the transposition operator to \a r.
    expression transposition(const expression& r);

    /// A trie-like automaton (single initial state, multiple final
    /// states) to accept \a p.
    ///
    /// \param p    the polynomial that describes the (finite) series
    automaton trie(const polynomial& p);

    /// A trie-like automaton (single initial state, multiple final
    /// states) to accept the series contained in \a is.
    ///
    /// \param ctx     the (word-based) context used to read the words.
    /// \param is      the input stream.
    /// \param format  the format of the file: "words" or "monomials".
    automaton trie(const context& ctx, std::istream& is,
                   const std::string& format = "default");

    /// The trim subautomaton of \a aut.
    automaton trim(const automaton& aut);

    /// Build a two-tape automaton from two automata.
    ///
    /// \param lhs   the left automaton
    /// \param rhs   the right automaton
    automaton tuple(const automaton& lhs, const automaton& rhs);

    /// Build a k-tape automaton from k automata.
    ///
    /// \param as    the automata
    automaton tuple(const std::vector<automaton>& as);

    /// Tuple two contexts.
    context tuple(const context& lhs, const context& rhs);

    /// A k-tape context from k contexts.
    ///
    /// \param ctxs    the contexts
    context tuple(const std::vector<context>& ctxs);

    /// Build a k-tape expansion from k expansions.
    expansion tuple(const std::vector<expansion>& rs);

    /// Build a two-tape expression from two expressions.
    expression tuple(const expression& lhs, const expression& rhs);

    /// Build a k-tape expression from k expressions.
    expression tuple(const std::vector<expression>& rs);

    /// Build a k-tape polynomial from k polynomials.
    polynomial tuple(const std::vector<polynomial>& ps);

    /// The implementation type of a.
    std::string type(const automaton& a);

    /// The Brzozowski universal witness.
    automaton u(const context& ctx, unsigned n);

    /// The universal automaton of \a aut.
    automaton universal(const automaton& aut);

    /// The weight one.
    weight weight_one(const context& c);

    /// Compute weight of the series
    weight weight_series(const automaton& aut);

    /// The weight zero.
    weight weight_zero(const context& c);

    /// The ZPC automaton of \a exp.
    ///
    /// \param exp    the expression to build the automaton from.
    /// \param algo   the specific algorithm to use.
    ///               It can be "regular"/"auto" or the variant "compact".
    automaton zpc(const expression& exp, const std::string& algo = "auto");

    /// Output automaton \a a on \a o.
    std::ostream& operator<<(std::ostream& o, const automaton& a);

    /// Output context \a c on \a o.
    std::ostream& operator<<(std::ostream& o, const context& c);

    /// Output expansion \a e on \a o.
    std::ostream& operator<<(std::ostream& o, const expansion& e);

    /// Output expression \a r on \a o.
    std::ostream& operator<<(std::ostream& o, const expression& r);

    /// Output label \a l on \a o.
    std::ostream& operator<<(std::ostream& o, const label& l);

    /// Output polynomial \a p on \a o.
    std::ostream& operator<<(std::ostream& o, const polynomial& p);

    /// Output weight \a w on \a o.
    std::ostream& operator<<(std::ostream& o, const weight& w);
  }
}

#include <vcsn/dyn/algos.hxx>
