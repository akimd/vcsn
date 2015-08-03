#pragma once

#include <vector>

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  template <typename Context>
  mutable_automaton<Context>
  ladybird(const Context& ctx, unsigned n)
  {
    const auto& ls = *ctx.labelset();
    auto letters = detail::make_vector(ls.genset());
    require(3 <= letters.size(),
            "ladybird: the alphabet needs at least 3 letters");
    auto a = ls.value(letters[0]);
    auto b = ls.value(letters[1]);
    auto c = ls.value(letters[2]);

    auto res = make_mutable_automaton(ctx);

    auto s = res->new_state();
    res->set_initial(s);
    res->set_final(s);
    auto x = s;
    for (unsigned i = 1; i < n; ++i)
      {
        auto y = res->new_state();
        res->new_transition(x, y, a);
        res->new_transition(y, y, b);
        res->new_transition(y, y, c);
        res->new_transition(y, s, c);
        x = y;
      }
    res->new_transition(x, s, a);
    return res;
  }

  /*----------------.
  | dyn::ladybird.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename Unsigned>
      automaton
      ladybird(const dyn::context& ctx, unsigned n)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(::vcsn::ladybird(c, n));
      }
    }
  }
}
