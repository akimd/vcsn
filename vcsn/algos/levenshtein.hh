#pragma once

#include <vector>

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/weightset/fwd.hh>

namespace vcsn
{
  /// The Levenshtein automaton for a given context.
  template <typename Context>
  mutable_automaton<Context>
  levenshtein(const Context& ctx)
  {
    static_assert(Context::is_lat,
                  "levenshtein: labelset must be a tupleset");
    static_assert(Context::labelset_t::size() == 2,
                  "levenshtein: labelset must have 2 tapes");
    static_assert(Context::labelset_t::template valueset_t<0>::has_one(),
                  "levenshtein: first tape must have empty word");
    static_assert(Context::labelset_t::template valueset_t<1>::has_one(),
                  "levenshtein: second tape must have empty word");
    static_assert(std::is_same<typename Context::weightset_t, nmin>::value,
                  "levenshtein: weightset must be nmin");
    using label_t = typename Context::labelset_t::value_t;
    const auto& ls = *ctx.labelset();
    const auto& ls1 = ls.template set<0>();
    const auto& ls2 = ls.template set<1>();
    const auto& ws = *ctx.weightset();
    auto letters1 = detail::make_vector(ls1.generators());
    auto letters2 = detail::make_vector(ls2.generators());

    auto res = make_mutable_automaton(ctx);

    auto s = res->new_state();
    res->set_initial(s);
    res->set_final(s);
    // Suppressions.
    for (auto l : letters1)
      res->new_transition(s, s, label_t{l, ls2.one()}, 1);
    // Insertions.
    for (auto l : letters2)
      res->new_transition(s, s, label_t{ls1.one(), l}, 1);
    // Substitutions.
    for (auto l : letters1)
      for (auto l2 : letters2)
        res->new_transition(s, s, label_t{l, l2}, !ls1.equal(l, l2));
    return res;
  }

  /*-------------------.
  | dyn::levenshtein.  |
  `-------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context>
      automaton
      levenshtein(const dyn::context& ctx)
      {
        const auto& c = ctx->as<Context>();
        return ::vcsn::levenshtein(c);
      }
    }
  }
}
