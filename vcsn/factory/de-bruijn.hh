#ifndef VCSN_FACTORY_DE_BRUIJN_HH
# define VCSN_FACTORY_DE_BRUIJN_HH

# include <iterator>
# include <stdexcept>

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/context.hh>

namespace vcsn
{
  // (a+b)*a(a+b)^n.
  template <class Context>
  mutable_automaton<Context>
  de_bruijn(unsigned n, const Context& ctx)
  {
    static_assert(Context::is_lal || Context::is_lan,
                  "requires labels_are_letters or nullable");
    size_t sz =
      std::distance(std::begin(*ctx.labelset()), std::end(*ctx.labelset()));
    if (sz < 2)
      throw std::invalid_argument("de_bruijn: the alphabet needs"
                                  " at least 2 letters");
    using context_t = Context;
    mutable_automaton<context_t> res{ctx};

    auto init = res.new_state();
    res.set_initial(init);
    for (char l: *ctx.labelset())
      res.set_transition(init, init, l);

    auto prev = res.new_state();
    res.set_transition(init, prev, *std::begin(*ctx.labelset()));

    while (n--)
      {
        auto next = res.new_state();
        for (char l: *ctx.labelset())
          res.set_transition(prev, next, l);
        prev = next;
      }
    res.set_final(prev);
    return res;
  }

  /*-----------------.
  | dyn::de_bruijn.  |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Ctx>
      automaton
      de_bruijn(const dyn::context& ctx, unsigned n)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(de_bruijn<Ctx>(n, c));
      }

      REGISTER_DECLARE(de_bruijn,
                       (const dyn::context& ctx, unsigned n) -> automaton);
    }
  }

}

#endif // !VCSN_FACTORY_DE_BRUIJN_HH
