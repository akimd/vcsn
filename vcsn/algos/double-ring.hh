#ifndef VCSN_FACTORY_DOUBLE_RING_HH
# define VCSN_FACTORY_DOUBLE_RING_HH

# include <map>

# include <vcsn/ctx/traits.hh>
# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{
  template <class Context>
  mutable_automaton<Context>
  double_ring(const Context& ctx, unsigned n,
              const std::vector<unsigned>& finals)
  {
    using context_t = Context;
    const auto& gens = ctx.labelset()->genset();
    std::vector<typename context_t::labelset_t::letter_t> letters
      {std::begin(gens), std::end(gens)};
    require(2 <= letters.size(),
            "double_ring: the alphabet needs at least 2 letters");
    auto a = letters[0];
    auto b = letters[1];

    using automaton_t = mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);
    if (n == 0)
      return res;

    // Set initial.
    auto p = res->new_state();
    res->set_initial(p);
    // Have states start on base 0. No need for pre and post states here.
    std::map<unsigned, state_t> states;
    // We want first state to be 0 and not 2.
    states.emplace(0, p);
    // Set transitions.
    state_t x = p;
    for (unsigned i = 1; i < n; ++i)
      {
        state_t y = res->new_state();
        res->new_transition(x, y, a);
        res->new_transition(y, x, b);
        x = y;
        states.emplace(i, y);
      }
    res->new_transition(x, p, a);
    res->new_transition(p, x, b);

    // Add finals.
    for (auto f: finals)
    {
      require(f < n, "double_ring: invalid list of finals");
      res->set_final(states[f]);
    }

    return res;
  }

  /*-------------------.
  | dyn::double_ring.  |
  `-------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename, typename>
      automaton
      double_ring(const dyn::context& ctx, unsigned n,
                  const std::vector<unsigned>& f)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(double_ring<Ctx>(c, n, f));
      }

      REGISTER_DECLARE(double_ring,
                       (const context& ctx, unsigned n,
                        const std::vector<unsigned>& f) -> automaton);
    }
  }
}

#endif // !VCSN_FACTORY_DOUBLE_RING_HH
