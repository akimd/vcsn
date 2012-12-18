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
    ratexp aut_to_exp(const automaton& aut);
    ratexp aut_to_exp_in_degree(const automaton& aut);

    automaton de_bruijn(const context& ctx, unsigned n);

    automaton determinize(const automaton& aut);

    void dotty(const automaton& aut, std::ostream& out);
    std::string dotty(const automaton& aut);

    std::string eval(const automaton& aut, const std::string& s);

    automaton lift(const automaton& aut);

    context* make_context(const std::string& name, const std::string& gens);

    abstract_ratexpset* make_ratexpset(const context& ctx);

    automaton read_automaton_file(const std::string& f);
    automaton read_automaton_string(const std::string& s);

    std::ostream& print(const automaton& a, std::ostream& o);
    std::ostream& print(const ratexp& e, std::ostream& o);

    automaton standard_of(const ratexp& e);

    automaton transpose(automaton& aut);

    ratexp transpose(const ratexp& e);
  }
}

#endif // !VCSN_ALGOS_DYN_HH
