#pragma once

#include <iterator> // std::distance
#include <stdexcept>

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/algorithm.hh> // vcsn::front
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  // (a+b)*a(a+b)^n.
  template <typename Context>
  mutable_automaton<Context>
  de_bruijn(const Context& ctx, unsigned n)
  {
    const auto& ls = *ctx.labelset();
    const auto& gens = ls.generators();
    size_t sz = boost::distance(gens);
    require(2 <= sz, "de_bruijn: the alphabet needs at least 2 letters");
    using context_t = Context;
    using automaton_t = mutable_automaton<context_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    auto init = res->new_state();
    res->set_initial(init);
    for (auto l: gens)
      res->new_transition(init, init, ls.value(l));

    auto prev = res->new_state();
    res->new_transition(init, prev, ls.value(detail::front(gens)));

    while (n--)
      {
        auto next = res->new_state();
        for (auto l: gens)
          res->new_transition(prev, next, ls.value(l));
        prev = next;
      }
    res->set_final(prev);
    return res;
  }

  /*-----------------.
  | dyn::de_bruijn.  |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename Unsigned>
      automaton
      de_bruijn(const dyn::context& ctx, unsigned n)
      {
        const auto& c = ctx->as<Ctx>();
        return ::vcsn::de_bruijn(c, n);
      }
    }
  }

}
