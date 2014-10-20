#ifndef VCSN_DYN_ALGOS_HH
# define VCSN_DYN_ALGOS_HH

# include <iosfwd>
# include <string>
# include <vector>

# include <vcsn/algos/fwd.hh>
# include <vcsn/core/rat/identities.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/direction.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn LIBVCSN_API
  {
    /// The accessible subautomaton of \a aut.
    automaton accessible(const automaton& aut);

    /// An ambiguous word, or raise if there is none.
    label ambiguous_word(const automaton& aut);

    /// Whether define the same language.
    /// \pre The labelsets of lhs and rhs are free.
    /// \pre lhs and rhs are Boolean, or on Z, or on a field.
    bool are_equivalent(const automaton& lhs, const automaton& rhs);

    /// Whether define the same language.
    /// \pre The labelsets of lhs and rhs are free.
    /// \pre lhs and rhs are Boolean, or on Z, or on a field.
    bool are_equivalent(const ratexp& lhs, const ratexp& rhs);

    /// Whether there exists an isomorphism between the states of \a
    /// lhs and those of \a rhs.
    bool are_isomorphic(const automaton& lhs, const automaton& rhs);

    /// Focus on a specific tape of a tupleset automaton.
    automaton blind(automaton& aut, unsigned tape);

    /// Produce a Černý automaton of \a num_states states.
    automaton cerny(const context& ctx, unsigned num_states);

    /// Repeated concatenation of \a aut with itself.
    automaton chain(const automaton& aut, int min, int max);

    /// Repeated concatenation of \a e with itself.
    ratexp chain(const ratexp& e, int min, int max);

    /// The coaccessible subautomaton of \a aut.
    automaton coaccessible(const automaton& aut);

    /// The complement of \a aut.
    /// \pre aut is lal
    /// \pre aut is Boolean
    /// \pre aut is deterministic
    /// \pre aut is complete
    automaton complement(const automaton& aut);

    /// Add the complement operator to \a r.
    ratexp complement(const ratexp& r);

    /// A completed copy of \a aut.
    /// \pre aut is lal
    automaton complete(const automaton& aut);

    /// The composition of transducers \a lhs and \a rhs.
    automaton compose(automaton& lhs, automaton& rhs);

    /// Concatenate two standard automata.
    automaton concatenate(const automaton& lhs, const automaton& rhs);

    /// Concatenate two polynomials.
    polynomial concatenate(const polynomial& lhs, const polynomial& rhs);

    /// Concatenate two ratexps.
    ratexp concatenate(const ratexp& lhs, const ratexp& rhs);

    /// The Hadamard product of ratexps \a lhs and \a rhs.
    ratexp conjunction(const ratexp& lhs, const ratexp& rhs);

    /// The weight associated to the empty word in \a e.
    weight constant_term(const ratexp& e);

    /// The context of this automaton.
    context context_of(const automaton& a);

    /// The context of this ratexp.
    context context_of(const ratexp& r);

    /// A copy of \a aut.
    automaton copy(const automaton& aut);

    /// Copy of \a exp, but built with \a rs.
    ratexp copy(const ratexp& exp, const ratexpset& rs);

    /// A standardized transpositive \a a.
    automaton costandard(const automaton& a);

    /// A simple NFA for (a+b)*a(a+b)^n.
    automaton de_bruijn(const context& ctx, unsigned n);

    /// Derive \a exp with respect to \a s.
    /// \param exp       the input ratexp
    /// \param l         the label used for derivation
    /// \param breaking  whether to split the result
    /// \pre  \a exp must be LAL.
    polynomial derivation(const ratexp& exp, const label& l,
                          bool breaking = false);

    /// The derived-term automaton of \a e.
    /// \param e      the input ratexp
    /// \param algo   how the derived terms are computed:
    ///    "auto"                  alias for "expansion"
    ///    "derivation"            compute by derivation
    ///    "breaking_derivation"   compute by breaking derivation
    ///    "expansion"             compute by expansion
    ///    "breaking_expansion"    compute by breaking expansion
    automaton derived_term(const ratexp& e,
                           const std::string& algo = "auto");

    /// The determinized automaton.
    /// \param aut       the automaton to determinize
    /// \param algo
    ///     "boolean"     use efficient bitsets
    ///     "weighted"    accept non Boolean automata (might not terminate)
    ///     "auto"        "boolean" if the automaton is Boolean,
    ///                   "weighted" otherwise.
    /// \pre  the labelset of \a aut must be free.
    automaton determinize(const automaton& aut,
                          const std::string& algo  = "weighted");

    /// The codeterminized automaton.
    /// \param aut       the automaton to codeterminize
    /// \param algo
    ///     "boolean"     use efficient bitsets
    ///     "weighted"    accept non Boolean automata (might not terminate)
    ///     "auto"        "boolean" if the automaton is Boolean,
    ///                   "weighted" otherwise.
    /// \pre  the labelset of \a aut must be free.
    automaton codeterminize(const automaton& aut,
                            const std::string& algo  = "weighted");

    /// The cominimized automaton.
    /// \param aut       the automaton to cominimize
    /// \param algo      the specific algorithm to use
    /// \pre  \a aut must be LAL.
    /// \pre  \a aut must be deterministic.
    automaton cominimize(const automaton& aut,
                         const std::string& algo = "auto");

    /// An automaton whose behavior is that of \a lhs on words not
    /// accepted by \a rhs.
    /// \param lhs   a LAL automaton
    /// \param rhs   a LAL Boolean automaton
    /// \pre \a rhs is Boolean.
    automaton difference(const automaton& lhs, const automaton& rhs);

    /// Words accepted by \a lhs, but not by \a rhs.
    ratexp difference(const ratexp& lhs, const ratexp& rhs);

    /// An automaton which accepts a word n representing a number in
    /// base b iff k|n.
    automaton divkbaseb(const context& ctx, unsigned divisor, unsigned base);

    /// Output \a aut in GraphViz' Dot format.
    /// \param aut  the automaton to format
    /// \param out  the output stream where to print the dot file
    /// \param dot2tex
    ///    whether to generate LaTeX escapes in the dot file for
    ///    use with dot2tex.
    std::ostream& dot(const automaton& aut, std::ostream& out,
                      bool dot2tex = false);

    /// A string representing \a aut in GraphViz' Dot format.
    std::string dot(const automaton& aut);

    /// The double_ring automaton with \a n states and \a f the list of finals.
    automaton double_ring(const context& ctx, unsigned n,
                          const std::vector<unsigned>& f);

    /// The LAO automaton \a aut with state \a s removed.
    automaton eliminate_state(const automaton& aut, int s);

    /// Output in Extended FSM format.
    std::ostream& efsm(const automaton& aut, std::ostream& out);

    /// All the accepted words of at most \a max letters.
    polynomial enumerate(const automaton& aut, unsigned max);

    /// Evaluate \a s on \a aut.
    weight eval(const automaton& aut, const label& l);

    /// Distribute product over addition recursively under the starred
    /// subexpressions and group the equal monomials.
    ratexp expand(const ratexp& e);

    /// Create a factor automaton from \a aut.
    automaton factor(const automaton& aut);

    /// Output in FAdo format.
    std::ostream& fado(const automaton& aut, std::ostream& out);

    /// Output in Grail format.
    std::ostream& grail(const automaton& aut, std::ostream& out);

    /// Whether the automaton has the twins property.
    bool has_twins_property(const automaton& aut);

    /// The identities of ratexp \a exp.
    rat::identities identities(const ratexp& exp);

    /// The infiltration of automata \a lhs and \a rhs.
    /// Performs the join of their types.
    automaton infiltration(const automaton& lhs, const automaton& rhs);

    /// The infiltration product of automata.
    /// Performs the join of their types.
    automaton infiltration(const std::vector<automaton>& as);

    /// Output various facts about an automaton.
    /// \param aut  the automaton under study
    /// \param out  the output stream where to print the info
    /// \param detailed
    ///    whether to print even facts costly to compute.
    std::ostream& info(const automaton& aut, std::ostream& out,
                       bool detailed = false);

    /// Output various facts about \a exp.
    std::ostream& info(const ratexp& exp, std::ostream& out);

    /// Split automaton on the incoming transition.
    /// Every state either has only epsilon incoming transitions
    /// or non-epsilon ones.
    /// \pre aut is lal or lan
    automaton insplit(const automaton& aut);

    /// Whether \a aut is accessible.
    bool is_accessible(const automaton& aut);

    /// Whether \a aut is ambiguous.
    /// \pre \a aut is LAL.
    bool is_ambiguous(const automaton& aut);

    /// Whether \a aut is coaccessible.
    bool is_coaccessible(const automaton& aut);

    /// Whether \a aut is codeterministic.
    /// \pre \a aut is LAL.
    bool is_codeterministic(const automaton& aut);

    /// Whether \a aut is complete.
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
    /// \pre \a aut is a transducer.
    bool is_functional(const automaton& aut);

    /// Whether is normalized (in the Thompson sense), i.e., standard
    /// and co-standard.
    bool is_normalized(const automaton& aut);

    /// Whether the outgoing transitions of each state have increasing labels.
    bool is_out_sorted(const automaton& aut);

    /// Whether has no spontaneous transition.
    bool is_proper(const automaton& aut);

    /// Whether is standard (unique initial state, with weight one, no
    /// incoming transition).
    bool is_standard(const automaton& aut);

    /// Whether the word synchronizes aut.
    bool is_synchronized_by(const automaton& aut, const label& word);

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
    bool is_valid(const ratexp& e);

    /// The minimized automaton.
    /// \param aut       the automaton to minimize
    /// \param algo      the specific algorithm to use
    /// \pre  \a aut must be LAL.
    /// \pre  \a aut must be deterministic.
    automaton minimize(const automaton& aut,
                       const std::string& algo = "auto");

    /// The ladybird automaton with \a n states.
    automaton ladybird(const context& ctx, unsigned n);

    /// The left-multiplication of an automaton with \a w as weight.
    /// \pre aut is standard.
    automaton left_mult(const weight& w, const automaton& aut);

    /// The left-multiplication of a ratexp with \a w as weight.
    ratexp left_mult(const weight& w, const ratexp& aut);

    /// The lifted LAO automaton from \a aut.
    automaton lift(const automaton& aut);

    /// The lifted LAO rational expression from \a e.
    ratexp lift(const ratexp& e);

    /// Build a context from its name.
    context make_context(const std::string& name);

    /// Build an automatonset from its context.
    automaton_editor* make_automaton_editor(const context& ctx);

    /// Build an ratexpset from its context.
    ratexpset make_ratexpset(const context& ctx, ::vcsn::rat::identities is);

    /// The context for words.
    context make_word_context(const context& ctx);

    /// Multiply two weights.
    weight multiply(const weight& lhs, const weight& rhs);

    /// Normalize automaton \a aut.
    automaton normalize(const automaton& aut);

    /// Build the pair automaton of the given automaton
    automaton pair(const automaton& aut, bool keep_initials = false);

    /// Create a prefix automaton from \a aut.
    automaton prefix(const automaton& aut);

    /// Repeated product of \a aut with itself.
    automaton power(const automaton& aut, unsigned n);

    /// Print automaton \a a on \a o using format \a format.
    std::ostream& print(const automaton& a, std::ostream& o,
                        const std::string& format = "default");

    /// Print context \a c on \a o using format \a format.
    std::ostream& print(const context& c, std::ostream& o,
                        const std::string& format = "default");

    /// Print expansion \a e on \a o using format \a format.
    std::ostream& print(const expansion& e, std::ostream& o,
                        const std::string& format = "default");

    /// Print label \a l on \a o using format \a format.
    std::ostream& print(const label& l, std::ostream& o,
                        const std::string& format = "default");

    /// Print polynomial \a p on \a o using format \a format.
    std::ostream& print(const polynomial& p, std::ostream& o,
                        const std::string& format = "default");

    /// Print ratexp \a e on \a o using format \a format.
    std::ostream& print(const ratexp& e, std::ostream& o,
                        const std::string& format = "default");

    /// Print weight \a w on \a o using format \a format.
    std::ostream& print(const weight& w, std::ostream& o,
                        const std::string& format = "default");

    /// Specify the output format for \a o.
    void set_format(std::ostream& o, const std::string& format);

    /// Get the output format for \a o.
    std::string get_format(std::ostream& o);

    /// The product of automata \a lhs and \a rhs.
    /// Performs the meet of the contexts.
    automaton product(const automaton& lhs, const automaton& rhs);

    /// The product of automata.
    /// Performs the meet of the contexts.
    automaton product(const std::vector<automaton>& as);

    /// An equivalent automaton without spontaneous transitions.
    /// \param aut    the automaton in which to remove them
    /// \param dir    the direction of the closure
    /// \param prune  whether to delete states that become inaccessible
    automaton proper(const automaton& aut,
                     direction dir = direction::backward,
                     bool prune = true);

    /// The weight pushing automaton of \a aut.
    automaton push_weights(const automaton& aut);

    /// Produce a random automaton.
    /// \param ctx
    ///    The context of the result.
    /// \param num_states
    ///    The number of states wanted in the automata
    ///    (>0).  All states will be connected, and there will be no dead
    ///    state.
    /// \param density
    ///     The density of the automata.  This is the probability
    ///     (between 0.0 and 1.0), to add a transition between two
    ///     states.  All states have at least one outgoing transition,
    ///     so \a d is considered only when adding the remaining transition.
    ///     A density of 1 means all states will be connected to each other.
    /// \param num_initial
    ///    The number of initial states wanted (0 <= num_initial <= num_states)
    /// \param num_final
    ///    The number of final states wanted (0 <= num_final <= num_states)
    automaton random_automaton(const context& ctx,
                               unsigned num_states,
                               float density = 0.1,
                               unsigned num_initial = 1,
                               unsigned num_final = 1);

    /// Produce a random deterministic automaton.
    /// \param ctx
    ///    The context of the result.
    /// \param num_states
    ///    The number of states wanted in the automata (>0).
    automaton random_automaton_deterministic(const context& ctx,
                                             unsigned num_states);

    /// Read an automaton from a stream.
    /// \param is      the input stream.
    /// \param format  its format.
    automaton read_automaton(std::istream& is,
                             const std::string& format = "default");

    /// Read a label from a stream.
    /// \param is    the input stream.
    /// \param ctx   the context.
    label read_label(std::istream& is, const context& ctx);

    /// Read a ratexp from a stream.
    /// \param is      the input stream.
    /// \param rs      the ratexp's ratexpset.
    /// \param format  the ratexp's format.
    ratexp read_ratexp(std::istream& is, const ratexpset& rs,
                       const std::string& format = "default");

    /// Read a polynomial from a stream
    /// \param is    the stream to parse.
    /// \param ctx   its context, from which is built its polynomialset.
    polynomial read_polynomial(std::istream& is, const context& ctx);

    /// Read a weight from a stream.
    /// \param is    the input stream.
    /// \param ctx   the context.
    weight read_weight(std::istream& is, const context& ctx);

    /// Reduce \a aut.
    automaton reduce(const automaton& aut);

    /// The right-mult automaton with \a w as weight.
    /// \pre aut is standard.
    automaton right_mult(const automaton& aut, const weight& w);

    /// The right-multiplication of a ratexp with \a w as weight.
    ratexp right_mult(const ratexp& aut, const weight& w);

    /// The number of strongly connected components.
    std::size_t num_sccs(const automaton& aut);

    /// The at-most \a max first accepted words.
    polynomial shortest(const automaton& aut, unsigned max = 1);

    /// The shuffle product of automata \a lhs and \a rhs.
    /// Performs the join of their type.
    automaton shuffle(const automaton& lhs, const automaton& rhs);

    /// The shuffle product of automata.
    /// Performs the join of their types.
    automaton shuffle(const std::vector<automaton>& as);

    /// The shuffle product of ratexps \a lhs and \a rhs.
    /// Performs the join of their type.
    ratexp shuffle(const ratexp& lhs, const ratexp& rhs);

    /// A copy of \a a with normalized state numbers.
    automaton sort(const automaton& a);

    /// Break all the expressions in \a p.
    polynomial split(const polynomial& p);

    /// Break \a exp.
    polynomial split(const ratexp& exp);

    /// A standardized \a a.
    automaton standard(const automaton& a);

    /// The standard automaton of \a e.
    automaton standard(const ratexp& e);

    /// Star of a standard automaton.
    automaton star(const automaton& aut);

    /// Star height of a ratexp.
    unsigned star_height(const ratexp& rs);

    /// A normalized form where star is applied only to expression
    /// without null constant-term.
    ratexp star_normal_form(const ratexp& e);

    /// The automaton in \a a with its metadata layers removed.
    automaton strip(const automaton& a);

    /// Create a suffix automaton from \a aut.
    automaton suffix(const automaton& aut);

    /// Create a subword automaton from \a aut.
    ///
    /// For each non-spontaneous transition, add a spontaneous
    /// transition with same source, destination, and weight.
    automaton subword(const automaton& aut);

    /// Sum of two standard automata.
    automaton sum(const automaton& lhs, const automaton& rhs);

    /// Sum of two polynomials.
    polynomial sum(const polynomial& lhs, const polynomial& rhs);

    /// Sum of two ratexps.
    ratexp sum(const ratexp& lhs, const ratexp& rhs);

    /// Sum of two weights.
    weight sum(const weight& lhs, const weight& rhs);

    /// A synchronizing word, or raise if there is none.
    label synchronizing_word(const automaton& aut,
                             const std::string& algo = "greedy");

    /// The Thompson automaton of \a e.
    automaton thompson(const ratexp& e);

    /// Output \a aut in LaTeX's TikZ format.
    std::ostream& tikz(const automaton& aut, std::ostream& out);

    /// First order development of a \a exp.
    /// \param exp              the input ratexp
    expansion to_expansion(const ratexp& exp);

    /// A ratexp denoting the language of \a aut.
    ratexp to_expression(const automaton& aut,
                         const std::string& algo = "auto");

    /// Transpose \a aut.
    automaton transpose(automaton& aut);

    /// Transpose \a e.
    ratexp transpose(const ratexp& e);

    /// Add the transposition operator to \a r.
    ratexp transposition(const ratexp& r);

    /// The useful subautomaton of \a aut.
    automaton trim(const automaton& aut);

    /// The Brzozowski universal witness.
    automaton u(const context& ctx, unsigned n);

    /// Union of two automata (plain graph union).
    /// Performs the join of the contexts.
    automaton union_a(const automaton& lhs, const automaton& rhs);

    /// The universal automaton of \a aut.
    automaton universal(const automaton& aut);

  }
}

namespace std LIBVCSN_API
{
  /// Output automaton \a a on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::automaton& a);

  /// Output context \a c on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::context& c);

  /// Output expansion \a e on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::expansion& e);

  /// Output label \a l on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::label& l);

  /// Output polynomial \a p on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::polynomial& p);

  /// Output ratexp \a e on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::ratexp& e);

  /// Output weight \a w on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::weight& w);
}

#endif // !VCSN_DYN_ALGOS_HH
