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
      return detail::product_registry().call(vname(lhs, rhs), lhs, rhs);
    }

    /*---------.
    | shuffle. |
    `---------*/

    REGISTER_DEFINE2(shuffle);

    automaton
    shuffle(const automaton& lhs, const automaton& rhs)
    {
      return detail::shuffle_registry().call(vname(lhs, rhs), lhs, rhs);
    }

    /*------------.
    | infiltrate. |
    `------------*/

    REGISTER_DEFINE2(infiltrate);

    automaton
    infiltrate(const automaton& lhs, const automaton& rhs)
    {
      return detail::infiltrate_registry().call(vname(lhs, rhs), lhs, rhs);
    }

    /*--------.
    | power.  |
    `--------*/

    REGISTER_DEFINE(power);

    automaton
    power(const automaton& aut, unsigned n)
    {
      return detail::power_registry().call(aut->vname(false), aut, n);
    }
  }
}
