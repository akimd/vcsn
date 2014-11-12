#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/left-mult.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(left_mult);
    automaton
    left_mult(const weight& w, const automaton& aut)
    {
      return detail::left_mult_registry().call(w, aut);
    }

    REGISTER_DEFINE(left_mult_expression);
    expression
    left_mult(const weight& w, const expression& r)
    {
      return detail::left_mult_expression_registry().call(w, r);
    }

    REGISTER_DEFINE(right_mult);
    automaton
    right_mult(const automaton& aut, const weight& w)
    {
      return detail::right_mult_registry().call(aut, w);
    }

    REGISTER_DEFINE(right_mult_expression);
    expression
    right_mult(const expression& r, const weight& w)
    {
      return detail::right_mult_expression_registry().call(r, w);
    }
  }
}
