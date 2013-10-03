#ifndef VCSN_FACTORY_LADYBIRD_HH
# define VCSN_FACTORY_LADYBIRD_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{
  template <class Context>
  mutable_automaton<Context>
  ladybird(unsigned n, const Context& ctx)
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
        res.add_transition(x, y, 'a');
        res.add_transition(y, y, 'b');
        res.add_transition(y, y, 'c');
        res.add_transition(y, p, 'c');
        x = y;
      }
    res.add_transition(x, p, 'a');
    return res;
  }

  /*----------------.
  | dyn::ladybird.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Ctx>
      automaton
      ladybird(const dyn::context& ctx, unsigned n)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(c, ladybird<Ctx>(n, c));
      }

      REGISTER_DECLARE(ladybird,
                       (const dyn::context& ctx, unsigned n) -> automaton);
    }
  }
}

#endif // !VCSN_FACTORY_LADYBIRD_HH
