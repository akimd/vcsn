#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(cerny);

  /*--------.
  | cerny.  |
  `--------*/

  /// Cerny automata are automata whose synchronizing word length is always
  /// (n - 1)^2, the upper bound of the Cerny's conjecture.
  ///
  /// Their transition function d(q, l) is defined by:
  ///
  /// - (q + 1) % n  if l == a
  /// - q            if l != a and q != n - 1
  /// - 0            if l != a and q == n - 1

  template <typename Ctx>
  mutable_automaton<Ctx>
  cerny(const Ctx& ctx, unsigned num_states)
  {
    require(0 < num_states, "num_states must be > 0");

    using automaton_t = mutable_automaton<Ctx>;
    using state_t = state_t_of<automaton_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    std::vector<state_t> states;
    states.reserve(num_states);

    for (unsigned i = 0; i < num_states; ++i)
      states.push_back(res->new_state());

    for (unsigned i = 0; i < num_states; ++i)
      {
        bool la = true;
        for (auto l : ctx.labelset()->generators())
          {
            auto dest = (la || i == num_states - 1) ? (i + 1) % num_states : i;
            res->add_transition(states[i], states[dest], l,
                                ctx.weightset()->one());
            la = false;
          }
      }

    res->set_initial(states[0]);
    res->set_final(states[0]);
    res->properties().update(cerny_ftag{});

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename>
      automaton
      cerny(const context& ctx, unsigned num_states)
      {
        const auto& c = ctx->as<Ctx>();
        return vcsn::cerny(c, num_states);
      }
    }
  }
}
