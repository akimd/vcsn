#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/vector.hh> // make_vector

namespace vcsn
{
  /*------------------------------.
  | Function tag and properties.  |
  `------------------------------*/

  CREATE_FUNCTION_TAG(ladybird);

  template <>
  struct function_prop<ladybird_ftag>
  {
    // New automaton, no need to invalidate.
    static const bool invalidate = false;

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
          // By construction, a ladybird automaton is not deterministic.
          { is_deterministic_ptag::id(), boost::any{false} }
        });
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
      return updated_prop;
    }
  };

   /// Build the ladybird automaton of \a n states.
  template <typename Context>
  mutable_automaton<Context>
  ladybird(const Context& ctx, unsigned n)
  {
    const auto& ls = *ctx.labelset();
    auto letters = detail::make_vector(ls.generators());
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
    res->properties().update(ladybird_ftag{});
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
        return ::vcsn::ladybird(c, n);
      }
    }
  }
}
