#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/sum.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-----.
    | sum. |
    `-----*/

    namespace detail
    {
      static
      Registry<sum_t>&
      sum_registry()
      {
        static Registry<sum_t> instance{"sum"};
        return instance;
      }

      bool sum_register(const std::string& lctx, const std::string& rctx,
                            sum_t fn)
      {
        return sum_registry().set(lctx + " x " + rctx, fn);
      }
    }

    automaton
    sum(const automaton& lhs, const automaton& rhs)
    {
      return detail::sum_registry().call(rhs->vname(false) + " x " +
                                                 rhs->vname(false), lhs, rhs);
    }
  }
}
