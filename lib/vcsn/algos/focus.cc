#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {

    /*---------------.
    | conjunction.   |
    `---------------*/

    REGISTER_DEFINE(conjunction_vector);
    automaton
    conjunction(const std::vector<automaton>& as, bool lazy)
    {
      return detail::conjunction_vector_registry().call(as, lazy);
    }

    // Implement the binary case on top of the variadic one, to avoid
    // compiling it twice.
    automaton
    conjunction(const automaton& lhs, const automaton& rhs, bool lazy)
    {
      auto auts = std::vector<dyn::automaton>{lhs, rhs};
      return conjunction(auts, lazy);
    }


    /*---------.
    | focus.   |
    `---------*/

    static
    integral_constant to_integral_constant(unsigned tape)
    {
      return integral_constant{symbol("std::integral_constant<unsigned, "
                                      + std::to_string(tape) + '>')};
    }

    REGISTER_DEFINE(focus_context);
    context
    focus(const context& ctx, unsigned tape)
    {
      auto t = to_integral_constant(tape);
      return detail::focus_context_registry().call(ctx, t);
    }

    REGISTER_DEFINE(focus);
    automaton
    focus(const automaton& aut, unsigned tape)
    {
      auto t = to_integral_constant(tape);
      return detail::focus_registry().call(aut, t);
    }


    /*----------------.
    | infiltration.   |
    `----------------*/

    automaton
    infiltration(const automaton& lhs, const automaton& rhs)
    {
      auto auts = std::vector<dyn::automaton>{lhs, rhs};
      return infiltration(auts);
    }


    /*-----------.
    | shuffle.   |
    `-----------*/

    automaton
    shuffle(const automaton& lhs, const automaton& rhs)
    {
      auto auts = std::vector<dyn::automaton>{lhs, rhs};
      return shuffle(auts);
    }


    /*---------.
    | tuple.   |
    `---------*/

    expression
    tuple(const expression& lhs, const expression& rhs)
    {
      auto auts = std::vector<dyn::expression>{lhs, rhs};
      return tuple(auts);
    }
  }
}
