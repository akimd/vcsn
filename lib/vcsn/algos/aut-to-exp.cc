#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/aut_to_exp.hh>
#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  namespace dyn
  {

    /*-------------.
    | aut_to_exp.  |
    `-------------*/

    namespace details
    {
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
    }

    rat::exp_t
    aut_to_exp(const dyn::abstract_automaton& aut)
    {
      return details::aut_to_exp_registry().call(aut.vname(),
                                                 aut);
    }

    /*-----------------------.
    | aut_to_exp_in_degree.  |
    `-----------------------*/

    namespace details
    {
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
    }

    rat::exp_t
    aut_to_exp_in_degree(const dyn::abstract_automaton& aut)
    {
      return details::aut_to_exp_in_degree_registry().call(aut.vname(),
                                                           aut);
    }
  }
}
