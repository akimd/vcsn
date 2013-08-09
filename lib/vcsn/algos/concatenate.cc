#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/concatenate.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------------.
    | concatenate. |
    `-------------*/

    namespace detail
    {
      static
      Registry<concatenate_t>&
      concatenate_registry()
      {
        static Registry<concatenate_t> instance{"concatenate"};
        return instance;
      }

      bool concatenate_register(const std::string& lctx, const std::string& rctx,
                            concatenate_t fn)
      {
        return concatenate_registry().set(lctx + " x " + rctx, fn);
      }
    }

    automaton
    concatenate(const automaton& lhs, const automaton& rhs)
    {
      return detail::concatenate_registry().call(rhs->vname(false) + " x " +
                                                 rhs->vname(false), lhs, rhs);
    }
  }
}
