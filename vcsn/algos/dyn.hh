#ifndef VCSN_ALGOS_DYN_HH
# define VCSN_ALGOS_DYN_HH

# include <iosfwd>
# include <vcsn/core/fwd.hh>
# include <vcsn/core/rat/fwd.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/ctx/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    enum class FileType
    {
      fsm,
      dot,
      text,
      xml
    };

    void fsm(const automaton& aut, std::ostream& out);
    std::string fsm(const automaton& aut);

    ratexp aut_to_exp(const automaton& aut);
    ratexp aut_to_exp_in_degree(const automaton& aut);

    automaton de_bruijn(const context& ctx, unsigned n);

    automaton determinize(const automaton& aut);

    void dot(const automaton& aut, std::ostream& out);
    std::string dot(const automaton& aut);

    std::string eval(const automaton& aut, const std::string& s);

    automaton ladybird(const context& ctx, unsigned n);

    automaton lift(const automaton& aut);
    ratexp lift(const ratexp& aut);

    context make_context(const std::string& name);

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

    void xml(const dyn::ratexp& e, std::ostream& out);
    std::string xml(const dyn::ratexp& e);
  }
}

#endif // !VCSN_ALGOS_DYN_HH
