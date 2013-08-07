#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/union.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    /*---------.
    | union_a. |
    `---------*/

    namespace detail
    {
      static
      Registry<union_a_t>&
      union_a_registry()
      {
        static Registry<union_a_t> instance{"union_a"};
        return instance;
      }

      bool union_a_register(const std::string& lctx, const std::string& rctx,
                            union_a_t fn)
      {
        return union_a_registry().set(lctx + " x " + rctx, fn);
      }
    }

    automaton
    union_a(const automaton& lhs, const automaton& rhs)
    {
      return detail::union_a_registry().call(rhs->vname(false) + " x " +
                                                 rhs->vname(false), lhs, rhs);
    }
  }
}
