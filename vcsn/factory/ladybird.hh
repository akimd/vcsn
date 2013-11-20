#ifndef VCSN_FACTORY_LADYBIRD_HH
# define VCSN_FACTORY_LADYBIRD_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/context.hh>

namespace vcsn
{
  template <class Context>
  mutable_automaton<Context>
  ladybird(const Context& ctx, unsigned n)
  {
    static_assert(Context::is_lal || Context::is_lan,
                  "requires labels_are_letters or nullable");
    using context_t = Context;
    mutable_automaton<context_t> res{ctx};

    auto p = res.new_state();
    res.set_initial(p);
    res.set_final(p);
    auto x = p;
    for (unsigned i = 1; i < n; ++i)
      {
        auto y = res.new_state();
        res.new_transition(x, y, 'a');
        res.new_transition(y, y, 'b');
        res.new_transition(y, y, 'c');
        res.new_transition(y, p, 'c');
        x = y;
      }
    res.new_transition(x, p, 'a');
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
        return make_automaton(ladybird<Ctx>(c, n));
      }

      REGISTER_DECLARE2(ladybird,
                       (const dyn::context& ctx, unsigned n) -> automaton);
    }
  }
}

#endif // !VCSN_FACTORY_LADYBIRD_HH
