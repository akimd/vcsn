#ifndef VCSN_FACTORY_DE_BRUIJN_HH
# define VCSN_FACTORY_DE_BRUIJN_HH

# include <iterator> // std::distance
# include <stdexcept>

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  // (a+b)*a(a+b)^n.
  template <typename Context>
  mutable_automaton<Context>
  de_bruijn(const Context& ctx, unsigned n)
  {
    const auto& gens = ctx.labelset()->genset();
    size_t sz = std::distance(std::begin(gens), std::end(gens));
    require(2 <= sz, "de_bruijn: the alphabet needs at least 2 letters");
    using context_t = Context;
    using automaton_t = mutable_automaton<context_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    auto init = res->new_state();
    res->set_initial(init);
    for (auto l: gens)
      res->new_transition(init, init, l);

    auto prev = res->new_state();
    res->new_transition(init, prev, *std::begin(gens));

    while (n--)
      {
        auto next = res->new_state();
        for (auto l: gens)
          res->new_transition(prev, next, l);
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
        return make_automaton(de_bruijn<Ctx>(c, n));
      }

      REGISTER_DECLARE(de_bruijn,
                       (const context& ctx, unsigned n) -> automaton);
    }
  }

}

#endif // !VCSN_FACTORY_DE_BRUIJN_HH
