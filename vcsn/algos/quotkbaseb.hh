#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/vector.hh> // make_vector

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(quotkbaseb);

  /// Build the transducer which accepts a word n representing a number in base
  /// "base" and outputs the quotient of the division of n by k.
  template <typename Context>
  mutable_automaton<Context>
  quotkbaseb(const Context& ctx, unsigned divisor, unsigned base)
  {
    // Highly redundant with divkbaseb.
    using context_t = Context;
    using automaton_t = mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    const auto& ls = *ctx.labelset();
    const auto& gens = ls.generators();
    auto lgens = detail::make_vector(ls.template set<0>().generators());
    auto rgens = detail::make_vector(ls.template set<1>().generators());

    require(divisor,
            "quotkbaseb: divisor cannot be 0");
    require(2 <= base,
            "quotkbaseb: base (", base, ") must be at least 2");
    VCSN_REQUIRE(base <= lgens.size(),
                 "quotkbaseb: base (", base,
                 ") must be less than or equal to the left alphabet size (",
                 lgens.size(), ')');
    VCSN_REQUIRE(base <= rgens.size(),
                 "quotkbaseb: base (", base,
                 ") must be less than or equal to the right alphabet size (",
                 rgens.size(), ')');

    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    // Add one state for each possible remainder. The last state encountered
    // during the evaluation will be n % k. If the last state is the state 0,
    // it means that the residue is 0, ie the word will be accepted, ie the
    // number is a multiple of k.
    std::vector<state_t> states;
    for (unsigned i = 0; i < divisor; ++i)
      states.emplace_back(res->new_state());

    res->set_initial(states[0]);
    res->set_final(states[0]);

    for (unsigned i = 0; i < divisor; ++i)
      {
        int e = i * base;
        for (unsigned l = 0; l < base; ++l)
          {
            int d = (e + l) % divisor;
            int f = (e + l) / divisor;
            res->new_transition(states[i], states[d],
                                ls.tuple(lgens[l], rgens[f]));
          }
      }
    res->properties().update(quotkbaseb_ftag{});
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename Unsigned1, typename Unsigned2>
      automaton
      quotkbaseb(const context& ctx, unsigned divisor, unsigned base)
      {
        const auto& c = ctx->as<Ctx>();
        return ::vcsn::quotkbaseb(c, divisor, base);
      }
    }
  }
}
