#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/is_complete.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*--------------.
  | is_complete.  |
  `--------------*/

  namespace dyn
  {
    namespace details
    {
      Registry<is_complete_t>&
        is_complete_registry()
        {
          static Registry<is_complete_t> instance{"is_complete"};
          return instance;
        }

      bool is_complete_register(const std::string& ctx,
          const is_complete_t& fn)
      {
        return is_complete_registry().set(ctx, fn);
      }
    }

    bool is_complete(const automaton& aut)
    {
      return details::is_complete_registry().call(aut->vname(),
          aut);
    }
  }
}
