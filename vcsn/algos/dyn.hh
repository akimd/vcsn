#ifndef VCSN_ALGOS_DYN_HH
# define VCSN_ALGOS_DYN_HH

# include <vcsn/core/fwd.hh>
# include <vcsn/core/rat/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    rat::exp_t aut_to_exp(const abstract_mutable_automaton& aut);
    rat::exp_t aut_to_exp_in_degree(const abstract_mutable_automaton& aut);
  }
}

#endif // !VCSN_ALGOS_DYN_HH
