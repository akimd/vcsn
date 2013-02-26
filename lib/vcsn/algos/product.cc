#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/product.hh>

namespace vcsn
{

  /*----------.
  | product.  |
  `----------*/

  namespace dyn
  {
    namespace details
    {
      Registry<product_t>&
      product_registry()
      {
        static Registry<product_t> instance{"product"};
        return instance;
      }

      bool product_register(const std::string& lctx, const std::string& rctx,
                            const product_t& fn)
      {
        return product_registry().set(lctx + " x " + rctx, fn);
      }
    }

    automaton
    product(const automaton& lhs, const automaton& rhs)
    {
      return
        details::product_registry().call(lhs->vname() + " x " + rhs->vname(),
                                         lhs, rhs);
    }
  }
}
