#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/accessible.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*-------------.
  | accessible.  |
  `-------------*/

  namespace dyn
  {
    namespace details
    {
      Registry<accessible_t>&
      accessible_registry()
      {
        static Registry<accessible_t> instance{"accessible"};
        return instance;
      }

      bool accessible_register(const std::string& ctx, const accessible_t& fn)
      {
        return accessible_registry().set(ctx, fn);
      }
    }

    automaton
    accessible(const automaton& aut)
    {
      return details::accessible_registry().call(aut->vname(),
                                                  aut);
    }
  }
}
