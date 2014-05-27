#ifndef VCSN_FACTORY_LADYBIRD_HH
# define VCSN_FACTORY_LADYBIRD_HH

# include <vector>

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  template <class Context>
  mutable_automaton<Context>
  ladybird(const Context& ctx, unsigned n)
  {
    using context_t = Context;
    const auto& gens = ctx.labelset()->genset();
    std::vector<typename context_t::labelset_t::letter_t> letters
      {std::begin(gens), std::end(gens)};
    require(3 <= letters.size(),
            "ladybird: the alphabet needs at least 3 letters");
    auto a = letters[0];
    auto b = letters[1];
    auto c = letters[2];

    using automaton_t = mutable_automaton<Context>;
    automaton_t res = std::make_shared<typename automaton_t::element_type>(ctx);

    auto p = res->new_state();
    res->set_initial(p);
    res->set_final(p);
    auto x = p;
    for (unsigned i = 1; i < n; ++i)
      {
        auto y = res->new_state();
        res->new_transition(x, y, a);
        res->new_transition(y, y, b);
        res->new_transition(y, y, c);
        res->new_transition(y, p, c);
        x = y;
      }
    res->new_transition(x, p, a);
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

      REGISTER_DECLARE(ladybird,
                       (const dyn::context& ctx, unsigned n) -> automaton);
    }
  }
}

#endif // !VCSN_FACTORY_LADYBIRD_HH
