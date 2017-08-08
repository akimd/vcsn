#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/algorithm.hh> // vcsn::front
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /*------------------------------.
  | Function tag and properties.  |
  `------------------------------*/

  struct de_bruijn_ftag{};

  template <>
  struct function_prop<de_bruijn_ftag>
  {
    // New automaton, no need to invalidate.
    static const bool invalidate = false;

    static auto& ignored_prop()
    {
      static auto ignored = ignored_prop_t{};
      return ignored;
    }

    static auto& updated_prop()
    {
#if defined __GNUC__ && ! defined __clang__
      // GCC 4.9 and 5.0 warnings: see
      // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
      static auto updated_prop = create_updated_prop(
        {
          // By construction, the de bruijn automaton is not deterministic.
          { is_deterministic_ptag::id(), boost::any{false} }
        });
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
      return updated_prop;
    }
  };

  /// Build a automaton for `(a+b)*a(a+b){n}`.
  template <typename Context>
  mutable_automaton<Context>
  de_bruijn(const Context& ctx, unsigned n)
  {
    const auto& ls = *ctx.labelset();
    const auto& gens = ls.generators();

    require(2 <= boost::distance(gens),
            "de_bruijn: ", ctx, ": the alphabet needs at least 2 letters");

    auto res = make_mutable_automaton(ctx);

    auto new_universal_transition =
      [&res, &gens, &ls](const auto src, const auto dst)
      {
        for (auto l: gens)
          res->new_transition(src, dst, ls.value(l));
        return dst;
      };

    const auto init = res->new_state();
    res->set_initial(init);
    new_universal_transition(init, init);

    auto last = res->new_state();
    res->new_transition(init, last, ls.value(detail::front(gens)));

    while (n--)
      last = new_universal_transition(last, res->new_state());

    res->set_final(last);
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
