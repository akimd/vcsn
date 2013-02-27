#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*----------------.
  | is_eps_acyclic. |
  `----------------*/

  namespace dyn
  {
    namespace details
    {
      Registry<is_eps_acyclic_t>&
      is_eps_acyclic_registry()
      {
        static Registry<is_eps_acyclic_t> instance{"is_eps_acyclic"};
        return instance;
      }

      bool
      is_eps_acyclic_register(const std::string& ctx,
              const is_eps_acyclic_t& f)
      {
        return is_eps_acyclic_registry().set(ctx, f);
      }
    }

    bool is_eps_acyclic(const automaton& aut)
    {
      return details::is_eps_acyclic_registry().call(aut->vname(), aut);
    }
  }
}
