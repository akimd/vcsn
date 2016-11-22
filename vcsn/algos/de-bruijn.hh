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
  /*------------------------------.
  | Function tag and properties.  |
  `------------------------------*/

  struct de_bruijn_ftag{};

  template <>
  struct function_prop<de_bruijn_ftag>
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
          // By construction, the de bruijn automaton is not deterministic.
          { is_deterministic_ptag::id(), boost::any{false} }
        });
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
      return updated_prop;
    }
  };

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
    res->properties().update(de_bruijn_ftag{});
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
