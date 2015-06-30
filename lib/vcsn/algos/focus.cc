#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
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
  }
}
