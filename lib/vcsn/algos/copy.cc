#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /*------.
  | copy. |
  `------*/

  namespace dyn
  {
    namespace details
    {
      Registry<copy_t>&
      copy_registry()
      {
        static Registry<copy_t> instance{"copy"};
        return instance;
      }

      bool copy_register(const std::string& ctx, const copy_t& fn)
      {
        return copy_registry().set(ctx, fn);
      }
    }

    automaton
    copy(const automaton& aut)
    {
      return details::copy_registry().call(aut->vname(), aut);
    }
  }
}
