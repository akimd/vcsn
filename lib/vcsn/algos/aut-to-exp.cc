#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/aut_to_exp.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*-------------.
  | aut_to_exp.  |
  `-------------*/

  Registry<aut_to_exp_t>&
  aut_to_exp_registry()
  {
    static Registry<aut_to_exp_t> instance{"aut_to_exp"};
    return instance;
  }

  bool aut_to_exp_register(const std::string& ctx, const aut_to_exp_t& fn)
  {
    return aut_to_exp_registry().set(ctx, fn);
  }

  rat::exp_t
  aut_to_exp(const abstract_mutable_automaton& aut)
  {
    return aut_to_exp_registry().call(aut.vname(),
                                      aut);
  }

  /*-----------------------.
  | aut_to_exp_in_degree.  |
  `-----------------------*/

  Registry<aut_to_exp_t>&
  aut_to_exp_in_degree_registry()
  {
    static Registry<aut_to_exp_t> instance{"aut_to_exp_in_degree"};
    return instance;
  }

  bool aut_to_exp_in_degree_register(const std::string& ctx,
                                     const aut_to_exp_t& fn)
  {
    return aut_to_exp_in_degree_registry().set(ctx, fn);
  }

  rat::exp_t
  aut_to_exp_in_degree(const abstract_mutable_automaton& aut)
  {
    return aut_to_exp_in_degree_registry().call(aut.vname(),
                                                aut);
  }
}
