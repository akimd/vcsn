#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  /*------------------.
  | is_deterministic.  |
  `------------------*/

  namespace dyn
  {
    namespace details
    {
      Registry<is_deterministic_t>&
      is_deterministic_registry()
      {
        static Registry<is_deterministic_t> instance{"is_deterministic"};
        return instance;
      }

      bool
      is_deterministic_register(const std::string& ctx,
                                const is_deterministic_t& fn)
      {
        return is_deterministic_registry().set(ctx, fn);
      }
    }

    bool
    is_deterministic(const automaton& aut)
    {
      return details::is_deterministic_registry().call(aut->vname(), aut);
    }
  }
}
