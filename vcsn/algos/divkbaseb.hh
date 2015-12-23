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

  /// Build the Boolean automaton which accepts a word n representing a
  /// number in base "base" if and only if divisor|n.
  template <typename Context>
  mutable_automaton<Context>
  divkbaseb(const Context& ctx, unsigned divisor, unsigned base)
  {
    using context_t = Context;
    using automaton_t = mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    const auto& ls = *ctx.labelset();
    const auto& gens = ls.generators();
    std::vector<label_t_of<context_t>> letters;
    for (auto l: gens)
      letters.emplace_back(ls.value(l));

    require(divisor,
            "divkbaseb: divisor cannot be 0");
    require(2 <= base,
            "divkbaseb: base (", base, ") must be at least 2");
    VCSN_REQUIRE(base <= letters.size(),
                 "divkbaseb: base (", base,
                 ") must be less than or equal to the alphabet size (",
                 letters.size(), ')');

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
            res->new_transition(states[i], states[d], letters[l]);
          }
      }
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename Unsigned1, typename Unsigned2>
      automaton
      divkbaseb(const context& ctx, unsigned divisor, unsigned base)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(::vcsn::divkbaseb(c, divisor, base));
      }
    }
  }
}
