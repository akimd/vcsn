#pragma once

#include <vcsn/ctx/traits.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{

  /// The Brzozowski universal witness.
  template <typename Context>
  mutable_automaton<Context>
  u(const Context& ctx, unsigned n)
  {
    using context_t = Context;
    using automaton_t = mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    require(2 <= n, "u: n must be at least 3");
    const auto& ls = *ctx.labelset();
    const auto& gens = ls.generators();
    std::vector<label_t_of<context_t>> letters;
    for (auto l: gens)
      letters.emplace_back(ls.value(l));
    require(3 <= letters.size(), "u: the alphabet needs at least 3 letters");
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    // The states.
    std::vector<state_t> states;
    for (unsigned i = 0; i < n; ++i)
      states.push_back(res->new_state());
    res->set_initial(states[0]);
    res->set_final(states[n-1]);

    // The 'a' transitions.
    auto a = letters[0];
    for (unsigned i = 0; i < n; ++i)
      res->new_transition(states[i], states[(i+1) % n], a);

    // The 'b' transitions.
    auto b = letters[1];
    res->new_transition(states[0], states[1], b);
    res->new_transition(states[1], states[0], b);
    for (unsigned i = 2; i < n; ++i)
      res->new_transition(states[i], states[i], b);

    // The 'c' transitions.
    auto c = letters[2];
    for (unsigned i = 0; i < n - 1; ++i)
      res->new_transition(states[i], states[i], c);
    res->new_transition(states[n - 1], states[0], c);

    return res;
  }

  /*---------.
  | dyn::u.  |
  `---------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename Unsigned>
      automaton
      u(const context& ctx, unsigned n)
      {
        const auto& c = ctx->as<Ctx>();
        return ::vcsn::u(c, n);
      }
    }
  }

}
