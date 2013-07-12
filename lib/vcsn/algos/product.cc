#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/product.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{

  /*----------.
  | product.  |
  `----------*/

  namespace dyn
  {
    namespace detail
    {
      static
      Registry<product_t>&
      product_registry()
      {
        static Registry<product_t> instance{"product"};
        return instance;
      }

      bool product_register(const std::string& lctx, const std::string& rctx,
                            product_t fn)
      {
        return product_registry().set(lctx + " x " + rctx, fn);
      }
    }

    automaton
    product(const automaton& lhs, const automaton& rhs)
    {
      return
        detail::product_registry().call(lhs->vname(false) + " x " + rhs->vname(false),
                                         lhs, rhs);
    }
  }
}
