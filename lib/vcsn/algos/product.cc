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

    REGISTER_DEFINE(product3);
    automaton
    product(const automaton& a, const automaton& b, const automaton& c)
    {
      return detail::product3_registry().call(a, b, c);
    }

    REGISTER_DEFINE(shuffle);
    automaton
    shuffle(const automaton& lhs, const automaton& rhs)
    {
      return detail::shuffle_registry().call(lhs, rhs);
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

    REGISTER_DEFINE(intersection_ratexp);
    ratexp
    intersection(const ratexp& lhs, const ratexp& rhs)
    {
      return detail::intersection_ratexp_registry().call(lhs, rhs);
    }
  }
}
