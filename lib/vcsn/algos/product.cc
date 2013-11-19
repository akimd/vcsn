#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/product.hh>
#include <vcsn/dyn/algos.hh>

namespace vcsn
{

  namespace dyn
  {
    REGISTER_DEFINE2(product);

    /*---------.
    | product. |
    `---------*/

    automaton
    product(const automaton& lhs, const automaton& rhs)
    {
      return detail::product_registry().call(lhs, rhs);
    }

    /*---------.
    | shuffle. |
    `---------*/

    REGISTER_DEFINE2(shuffle);

    automaton
    shuffle(const automaton& lhs, const automaton& rhs)
    {
      return detail::shuffle_registry().call(lhs, rhs);
    }

    /*---------------.
    | infiltration.  |
    `---------------*/

    REGISTER_DEFINE2(infiltration);

    automaton
    infiltration(const automaton& lhs, const automaton& rhs)
    {
      return detail::infiltration_registry().call(lhs, rhs);
    }

    /*--------.
    | power.  |
    `--------*/

    REGISTER_DEFINE2(power);

    automaton
    power(const automaton& aut, unsigned n)
    {
      return detail::power_registry().call(aut, n);
    }
  }
}
