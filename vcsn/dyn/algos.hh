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
    bool are_equivalent(const automaton& lhs, const automaton& rhs);
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

    automaton complete(const automaton& aut);

    automaton copy(const automaton& aut);

    automaton de_bruijn(const context& ctx, unsigned n);

    automaton determinize(const automaton& aut);

    std::ostream& dot(const automaton& aut, std::ostream& out);
    std::string dot(const automaton& aut);

    /// One of the shortest accepted words.
    /// \throw std::domain_error if the automaton is useless
    std::vector<std::string> enumerate(const automaton& aut, size_t max);

    dyn::weight eval(const automaton& aut, const std::string& s);

    automaton proper(const automaton& aut);

    std::ostream& info(const automaton& aut, std::ostream& out);
    std::ostream& info(const ratexp& exp, std::ostream& out);

    std::ostream& fsm(const automaton& aut, std::ostream& out);

    bool is_complete(const automaton& aut);

    bool is_deterministic(const automaton& aut);

    /// Whether has no state.
    bool is_empty(const automaton& aut);

    bool is_eps_acyclic(const automaton& aut);

    /// Whether is normalized (in the Thompson sense).
    bool is_normalized(const automaton& aut);

    bool is_proper(const automaton& aut);

    /// Whether has no useless state.
    bool is_trim(const automaton& aut);

    /// Whether has no useful state.
    bool is_useless(const automaton& aut);

    automaton ladybird(const context& ctx, unsigned n);

    automaton lift(const automaton& aut);
    ratexp lift(const ratexp& aut);

    context make_context(const std::string& name);

    automaton_editor* make_automaton_editor(const context& ctx);

    ratexpset make_ratexpset(const context& ctx);

    std::ostream& print(const automaton& a, std::ostream& o,
                        const std::string& type = "dot");
    std::ostream& print(const ratexp& e, std::ostream& o,
                        const std::string& type = "text");
    std::ostream& print(const weight& e, std::ostream& o,
                        const std::string& type = "text");

    automaton product(const automaton& lhs, const automaton& rhs);

    automaton read_automaton_file(const std::string& f);
    automaton read_automaton_string(const std::string& s);

    ratexp read_ratexp_file(const std::string& f, const context& ctx,
                            const std::string& type);
    ratexp read_ratexp_string(const std::string& s, const context& ctx,
                              const std::string& type);

    automaton standard(const ratexp& e);

    automaton thompson(const ratexp& e);

    std::ostream& tikz(const automaton& aut, std::ostream& out);

    automaton transpose(automaton& aut);
    ratexp transpose(const ratexp& e);

    /// The useful subautomaton of \a aut.
    automaton trim(const automaton& aut);

    /// One of the shortest accepted words.
    /// \throw std::domain_error if the automaton is useless
    std::string shortest(const automaton& aut);

    automaton universal(const automaton& aut);

    std::ostream& xml(const ratexp& e, std::ostream& out);
  }
}

#endif // !VCSN_DYN_ALGOS_HH
