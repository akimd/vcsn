#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/complete.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
    /*--------.
  | complete.  |
  `-----------*/

  namespace dyn
  {
    namespace details
    {
      Registry<complete_t>&
      complete_registry()
      {
        static Registry<complete_t> instance{"complete"};
        return instance;
      }

      bool
      complete_register(const std::string& ctx, const complete_t& fn)
      {
        return complete_registry().set(ctx, fn);
      }
    }

    automaton
    complete(const automaton& aut)
    {
      return details::complete_registry().call(aut->vname(), aut);
    }
  }
}
