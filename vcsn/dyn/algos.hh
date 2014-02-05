#ifndef VCSN_DYN_ALGOS_HH
# define VCSN_DYN_ALGOS_HH

# include <iosfwd>
# include <string>
# include <vector>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  // vcsn/algo/edit-automaton.hh.
  class automaton_editor;

  namespace dyn LIBVCSN_API
  {
    /// The accessible subautomaton of \a aut.
    automaton accessible(const automaton& aut);

    /// Whether define the same language.
    /// \pre lhs and rhs are lal
    /// \pre lhs and rhs are Boolean
    bool are_equivalent(const automaton& lhs, const automaton& rhs);

    /// Whether define the same language.
    bool are_equivalent(const ratexp& lhs, const ratexp& rhs);

    /// Whether there exists an isomorphism between \a lhs's states
    /// states and \a rhs's states.
    bool are_isomorphic(const automaton& lhs, const automaton& rhs);

    /// A ratexp denoting the language of \a aut.
    /// Uses the "naive" heuristics.
    ratexp aut_to_exp(const automaton& aut);

    /// A ratexp denoting the language of \a aut.
    ratexp aut_to_exp_in_degree(const automaton& aut);

    /// Concatenate \a n copies of \a aut.
    automaton chain(const automaton& aut, unsigned n);

    /// Repeated product of \a e with itself.
    ratexp chain(const ratexp& e, int min, int max);

    /// The coaccessible subautomaton of \a aut.
    automaton coaccessible(const automaton& aut);

    /// The complement of \a aut.
    /// \pre aut is lal
    /// \pre aut is Boolean
    /// \pre aut is deterministic
    /// \pre aut is complete
    automaton complement(const automaton& aut);

    /// A completed copy of \a aut.
    /// \pre aut is lal
    automaton complete(const automaton& aut);

    /// Concatenate two standard automata.
    automaton concatenate(const automaton& lhs, const automaton& rhs);

    /// Concatenate two ratexps.
    ratexp concatenate(const ratexp& lhs, const ratexp& rhs);

    /// The weight associated to the empty word in \a e.
    weight constant_term(const ratexp& e);

    /// A copy of \a aut.
    automaton copy(const automaton& aut);

    /// Copy of \a exp, but built with \a rs.
    ratexp copy(const ratexp& exp, const ratexpset& rs);

    /// A simple NFA for (a+b)*a(a+b)^n.
    automaton de_bruijn(const context& ctx, unsigned n);

    /// Derive \a exp with respect to \a s.
    /// \param exp       the input ratexp
    /// \param s         the string of labels used for derivation
    /// \param breaking  whether to split the result
    /// \pre  \a exp must be LAL.
    polynomial derivation(const ratexp& exp, const std::string& s,
                          bool breaking = false);

    /// The derived-term automaton of \a e.
    /// \param e         the input ratexp
    /// \param breaking  whether to split the result
    automaton derived_term(const ratexp& e, bool breaking = false);

    /// The determinized automaton.
    /// \param aut       the automaton to determinize
    /// \param complete  whether to force the result to be a complete automaton
    /// \pre  \a aut must be LAL.
    automaton determinize(const automaton& aut, bool complete = false);

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
    std::ostream& dot(const automaton& aut, std::ostream& out);

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
    weight eval(const automaton& aut, const std::string& s);

    /// Distribute product over addition recursively under the starred
    /// subexpressions and group the equal monomials.
    ratexp expand(const ratexp& e);

    /// Output in FAdo format.
    std::ostream& fado(const automaton& aut, std::ostream& out);

    /// First order development of a \a exp.
    /// \param exp              the input ratexp
    /// \param use_spontaenous  whether to generate "spontaneous" terms
    ///                         instead of recursive development
    polynomial first_order(const ratexp& exp, bool use_spontaenous = false);

    /// Output in Grail format.
    std::ostream& grail(const automaton& aut, std::ostream& out);

    /// The infiltration of automata \a lhs and \a rhs.
    /// Performs the union of the alphabets.
    automaton infiltration(const automaton& lhs, const automaton& rhs);

    /// Output various facts about \a aut.
    std::ostream& info(const automaton& aut, std::ostream& out);

    /// Output various facts about \a exp.
    std::ostream& info(const ratexp& exp, std::ostream& out);

    /// The Hadamard product of ratexps \a lhs and \a rhs.
    ratexp intersection(const ratexp& lhs, const ratexp& rhs);

    /// Whether \a aut is accessible.
    bool is_accessible(const automaton& aut);

    /// Whether \a aut is ambiguous.
    /// \pre \a aut is LAL.
    bool is_ambiguous(const automaton& aut);

    /// Whether \a aut is coaccessible.
    bool is_coaccessible(const automaton& aut);

    /// Whether \a aut is complete.
    /// \pre \a aut is LAL.
    bool is_complete(const automaton& aut);

    /// Whether \a aut is deterministic.
    /// \pre \a aut is LAL.
    bool is_deterministic(const automaton& aut);

    /// Whether has no state.
    bool is_empty(const automaton& aut);

    /// Whether has no cycle of spontaneous transitions.
    bool is_eps_acyclic(const automaton& aut);

    /// Whether is normalized (in the Thompson sense).
    bool is_normalized(const automaton& aut);

    /// Whether has no spontaneous transition.
    bool is_proper(const automaton& aut);

    /// Whether is standard (unique initial state, with weight one, no
    /// incoming transition).
    bool is_standard(const automaton& aut);

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
    /// \pre  \a aut must be LAL.
    /// \pre  \a aut must be deterministic.
    automaton minimize(const automaton& aut,
                       const std::string& algo = "signature");

    /// The ladybird automaton with \a n states.
    automaton ladybird(const context& ctx, unsigned n);

    /// The left-mult automaton with \a w as weight.
    /// \precondition aut is standard.
    automaton left_mult(const automaton& aut, const weight& w);

    /// The lifted LAO automaton from \a aut.
    automaton lift(const automaton& aut);

    /// The lifted LAO rational expression from \a e.
    ratexp lift(const ratexp& e);

    /// FIXME: The derived-term automaton of \a e.
    /// \param e         the input ratexp
    /// \param breaking  whether to split the result
    automaton linear(const ratexp& e, bool breaking = false);

    /// Build a context from its name.
    context make_context(const std::string& name);

    /// Build an automatonset from its context.
    automaton_editor* make_automaton_editor(const context& ctx);

    /// Build an ratexpset from its context.
    ratexpset make_ratexpset(const context& ctx);

    /// Repeated product of \a aut with itself.
    automaton power(const automaton& aut, unsigned n);

    /// Print automaton \a a on \a o using format \a format.
    std::ostream& print(const automaton& a, std::ostream& o,
                        const std::string& format = "default");

    /// Print context \a c on \a o using format \a format.
    std::ostream& print(const context& c, std::ostream& o,
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

    /// An equivalent automaton without spontaneous transitions.
    /// \param aut    the automaton in which to remove them
    /// \param prune  whether to delete states that become inaccessible
    automaton proper(const automaton& aut, bool prune = true);

    /// Produce a random automaton.
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

    /// Read an automaton from a stream.
    /// \param is    the input stream.
    /// \param format  its format.
    automaton read_automaton(std::istream& is,
                             const std::string& format = "default");

    /// Read a ratexp from a stream.
    /// \param is      the input stream.
    /// \param rs      the ratexp's ratexpset.
    /// \param format  the ratexp's format.
    ratexp read_ratexp(std::istream& is, const ratexpset& rs,
                       const std::string& format = "default");

    /// Read a polynomial in a stream
    /// \param is    the stream to parse.
    /// \param ctx   its context, from which is built its polynomialset.
    polynomial read_polynomial(std::istream& is, const context& ctx);

    /// Read a weight in a file.
    /// \param is    the input stream.
    /// \param ctx   the context, from which is built its weightset.
    weight read_weight(std::istream& is, const context& ctx);

    /// The right-mult automaton with \a w as weight.
    /// \precondition aut is standard.
    automaton right_mult(const automaton& aut, const weight& w);

    /// The at-most \a max first accepted words.
    polynomial shortest(const automaton& aut, unsigned max = 1);

    /// The shuffle of automata \a lhs and \a rhs.
    /// Performs the union of the alphabets.
    automaton shuffle(const automaton& lhs, const automaton& rhs);

    /// Break \a ratexp.
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

    /// Sum of two standard automata.
    automaton sum(const automaton& lhs, const automaton& rhs);

    /// Sum of two ratexps.
    ratexp sum(const ratexp& lhs, const ratexp& rhs);

    /// The Thompson automaton of \a e.
    automaton thompson(const ratexp& e);

    /// Output \a aut in LaTeX's TikZ format.
    std::ostream& tikz(const automaton& aut, std::ostream& out);

    /// Transpose \a aut.
    automaton transpose(automaton& aut);

    /// Transpose \a e.
    ratexp transpose(const ratexp& e);

    /// The useful subautomaton of \a aut.
    automaton trim(const automaton& aut);

    /// The Brzozowski universal witness.
    automaton u(const context& ctx, unsigned n);

    /// Union of two automata (plain graph union).
    /// Performs the union of the contexts.
    automaton union_a(const automaton& lhs, const automaton& rhs);

    /// The universal automaton of \a aut.
    automaton universal(const automaton& aut);
  }
}

namespace std LIBVCSN_API
{
  /// Output automaton \a a on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::automaton& a);

  /// Output polynomial \a p on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::polynomial& p);

  /// Output ratexp \a e on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::ratexp& e);

  /// Output weight \a w on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::weight& w);
}

#endif // !VCSN_DYN_ALGOS_HH
