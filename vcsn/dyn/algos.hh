#ifndef VCSN_DYN_ALGOS_HH
# define VCSN_DYN_ALGOS_HH

# include <iosfwd>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  // vcsn/algo/edit-automaton.hh.
  class automaton_editor;

  namespace dyn
  {
    enum class FileType
    {
      dot,
      fsm,
      null,
      text,
      xml
    };

    automaton accessible(const automaton& aut);

    ratexp aut_to_exp(const automaton& aut);
    ratexp aut_to_exp_in_degree(const automaton& aut);

    automaton complete(const automaton& aut);

    automaton de_bruijn(const context& ctx, unsigned n);

    automaton determinize(const automaton& aut);

    void dot(const automaton& aut, std::ostream& out);
    std::string dot(const automaton& aut);

    std::string eval(const automaton& aut, const std::string& s);

    automaton eps_removal(const automaton& aut);

    void fsm(const automaton& aut, std::ostream& out);
    std::string fsm(const automaton& aut);

    bool is_complete(const automaton& aut);

    bool is_deterministic(const automaton& aut);

    bool is_eps_acyclic(const automaton& aut);

    bool is_proper(const automaton& aut);

    automaton ladybird(const context& ctx, unsigned n);

    automaton lift(const automaton& aut);
    ratexp lift(const ratexp& aut);

    context make_context(const std::string& name);

    automaton_editor* make_automaton_editor(const context& ctx);

    ratexpset make_ratexpset(const context& ctx);

    std::ostream& print(const automaton& a, std::ostream& o, FileType type);
    std::ostream& print(const ratexp& e, std::ostream& o, FileType type);

    automaton product(const automaton& lhs, const automaton& rhs);

    automaton read_automaton_file(const std::string& f);
    automaton read_automaton_string(const std::string& s);

    ratexp read_ratexp_file(const std::string& f,
                            const context& ctx,
                            FileType type);
    ratexp read_ratexp_string(const std::string& s,
                              const context& ctx,
                              FileType type);

    automaton standard_of(const ratexp& e);

    automaton transpose(automaton& aut);
    ratexp transpose(const ratexp& e);

    void xml(const ratexp& e, std::ostream& out);
    std::string xml(const ratexp& e);
  }
}

#endif // !VCSN_DYN_ALGOS_HH
