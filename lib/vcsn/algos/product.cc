#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/product.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{

  namespace dyn
  {
    REGISTER_DEFINE(product);
    automaton
    product(const automaton& lhs, const automaton& rhs)
    {
      return detail::product_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(product_vector);
    automaton
    product(const std::vector<automaton>& auts)
    {
      return detail::product_vector_registry().call_variadic(auts);
    }

    REGISTER_DEFINE(shuffle);
    automaton
    shuffle(const automaton& lhs, const automaton& rhs)
    {
      return detail::shuffle_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(shuffle_vector);
    automaton
    shuffle(const std::vector<automaton>& auts)
    {
      return detail::shuffle_vector_registry().call_variadic(auts);
    }

    REGISTER_DEFINE(shuffle_ratexp);
    ratexp
    shuffle(const ratexp& lhs, const ratexp& rhs)
    {
      return detail::shuffle_ratexp_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(infiltration);
    automaton
    infiltration(const automaton& lhs, const automaton& rhs)
    {
      return detail::infiltration_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(power);
    automaton
    power(const automaton& aut, unsigned n)
    {
      return detail::power_registry().call(aut, n);
    }

    REGISTER_DEFINE(conjunction_ratexp);
    ratexp
    conjunction(const ratexp& lhs, const ratexp& rhs)
    {
      return detail::conjunction_ratexp_registry().call(lhs, rhs);
    }
  }
}
