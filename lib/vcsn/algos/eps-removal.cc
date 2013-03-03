#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/eps-removal.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*-------------.
  | eps-removal. |
  `-------------*/

  namespace dyn
  {
    namespace details
    {
      Registry<eps_removal_t>&
      eps_removal_registry()
      {
        static Registry<eps_removal_t> instance{"eps-removal"};
        return instance;
      }

      bool eps_removal_register(const std::string& ctx, const eps_removal_t& f)
      {
        return eps_removal_registry().set(ctx, f);
      }
    }

    automaton
    eps_removal(const automaton& aut)
    {
      return details::eps_removal_registry().call(aut->vname(), aut);
    }
  }
}
