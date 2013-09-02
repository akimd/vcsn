#ifndef VCSN_DYN_ALGOS_HH
# define VCSN_DYN_ALGOS_HH

# include <iosfwd>
# include <string>
# include <vector>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  // vcsn/algo/edit-automaton.hh.
  class automaton_editor;

  namespace dyn
  {
    /// The accessible subautomaton of \a aut.
    automaton accessible(const automaton& aut);

    /// Whether define the same language.
    /// \pre lhs and rhs are lal
    /// \pre lhs and rhs are Boolean
    bool are_equivalent(const automaton& lhs, const automaton& rhs);

    /// Whether define the same language.
    bool are_equivalent(const ratexp& lhs, const ratexp& rhs);

    ratexp aut_to_exp(const automaton& aut);
    ratexp aut_to_exp_in_degree(const automaton& aut);

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

    /// The weight associated to the empty word in \a e.
    dyn::weight constant_term(const ratexp& e);

    /// A copy of \a aut.
    automaton copy(const automaton& aut);

    /// A simple NFA for (a+b)*a(a+b)^n.
    automaton de_bruijn(const context& ctx, unsigned n);

    /// An automaton which accepts a word n representing a number in
    /// base b iff k|n.
    automaton divkbaseb(const context& ctx, unsigned divisor, unsigned base);

    /// Determinize \a aut.
    /// \pre  \a aut must be LAL.
    automaton determinize(const automaton& aut);

    /// Output \a aut in GraphViz' Dot format.
    std::ostream& dot(const automaton& aut, std::ostream& out);

    /// A string representing \a aut in GraphViz' Dot format.
    std::string dot(const automaton& aut);

    /// Output in Extended FSM format.
    std::ostream& efsm(const automaton& aut, std::ostream& out);

    /// One of the shortest accepted words.
    /// \throw std::domain_error if the automaton is useless
    std::vector<std::string> enumerate(const automaton& aut, size_t max);

    /// Evaluate \a s on \a aut.
    dyn::weight eval(const automaton& aut, const std::string& s);

    /// Output in FAdo format.
    std::ostream& fado(const automaton& aut, std::ostream& out);

    /// Output in Grail format.
    std::ostream& grail(const automaton& aut, std::ostream& out);

    /// Output various facts about \a aut.
    std::ostream& info(const automaton& aut, std::ostream& out);

    /// Output various facts about \a exp.
    std::ostream& info(const ratexp& exp, std::ostream& out);

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

    /// Whether rational expression is valid (all the starred
    /// sub-expressions are starrable).
    bool is_valid(const ratexp& e);

    /// The ladybird automaton with \a n states.
    automaton ladybird(const context& ctx, unsigned n);

    automaton lift(const automaton& aut);
    ratexp lift(const ratexp& aut);

    context make_context(const std::string& name);

    automaton_editor* make_automaton_editor(const context& ctx);

    ratexpset make_ratexpset(const context& ctx);

    /// Repeated product of \a aut with itself.
    automaton power(const automaton& aut, unsigned n);

    std::ostream& print(const automaton& a, std::ostream& o,
                        const std::string& type = "default");
    std::ostream& print(const ratexp& e, std::ostream& o,
                        const std::string& type = "default");
    std::ostream& print(const weight& e, std::ostream& o,
                        const std::string& type = "default");

    /// Specify the output format for \a o.
    void set_format(std::ostream& o, const std::string& type);

    /// Get the output format for \a o.
    std::string get_format(std::ostream& o);

    /// The product of automata \a lhs and \a rhs.
    automaton product(const automaton& lhs, const automaton& rhs);

    /// An automaton equivalent to \a aut, without spontaneous transitions.
    automaton proper(const automaton& aut);

    /// Read an automaton in a file.
    /// \param file  the file name.
    /// \param type  its format.
    automaton read_automaton_file(const std::string& file,
                                  const std::string& type = "dot");

    /// Read an automaton in a string.
    /// \param str  the automaton.
    automaton read_automaton_string(const std::string& str);

    ratexp read_ratexp_file(const std::string& f, const ratexpset& rs,
                            const std::string& type);
    ratexp read_ratexp_file(const std::string& f, const context& ctx,
                            const std::string& type);
    ratexp read_ratexp_string(const std::string& s, const context& ctx,
                              const std::string& type);

    /// A standardized \a a.
    automaton standard(const automaton& a);

    /// The standard automaton of \a e.
    automaton standard(const ratexp& e);

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

    /// One of the shortest accepted words.
    /// \throw std::domain_error if the automaton is useless
    std::string shortest(const automaton& aut);

    /// The Brzozowski universal witness.
    automaton u(const context& ctx, unsigned n);

    /// The universal automaton of \a aut.
    automaton universal(const automaton& aut);
  }
}

namespace std
{
  /// Output \a w on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::automaton& a);

  /// Output \a e on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::ratexp& e);

  /// Output \a w on \a o.
  std::ostream& operator<<(std::ostream& o, const vcsn::dyn::weight& w);
}

#endif // !VCSN_DYN_ALGOS_HH
