#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/are-equivalent.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{

  /*-----------------.
  | are_equivalent.  |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {
      static
      Registry<are_equivalent_t>&
      are_equivalent_registry()
      {
        static Registry<are_equivalent_t> instance{"are_equivalent"};
        return instance;
      }

      bool are_equivalent_register(const std::string& lctx,
                                   const std::string& rctx,
                                   are_equivalent_t fn)
      {
        return are_equivalent_registry().set(lctx + " x " + rctx, fn);
      }
    }

    bool
    are_equivalent(const automaton& lhs, const automaton& rhs)
    {
      return
        detail::are_equivalent_registry().call
        (lhs->vname() + " x " + rhs->vname(),
         lhs, rhs);
    }
  }
}
