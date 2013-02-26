#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/aut_to_exp.hh>
#include <vcsn/dyn/algos.hh>
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

    ratexp
    aut_to_exp(const automaton& aut)
    {
      return details::aut_to_exp_registry().call(aut->vname(),
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

    ratexp
    aut_to_exp_in_degree(const automaton& aut)
    {
      return details::aut_to_exp_in_degree_registry().call(aut->vname(),
                                                           aut);
    }
  }
}
