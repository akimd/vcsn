#ifndef VCSN_FACTORY_U_HH
# define VCSN_FACTORY_U_HH

# include <stdexcept>

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/context.hh>

namespace vcsn
{

  /// The Brzozowski universal witness.
  template <typename Context>
  mutable_automaton<Context>
  u(const Context& ctx, unsigned n)
  {
    using context_t = Context;
    using automaton_t = mutable_automaton<context_t>;
    using state_t = typename automaton_t::state_t;
    static_assert(Context::is_lal || Context::is_lan,
                  "requires labels_are_letters or nullable");
    if (n < 2)
      throw std::invalid_argument("u: n must be at least 3");
    std::vector<typename context_t::labelset_t::letter_t> letters
      {std::begin(*ctx.labelset()), std::end(*ctx.labelset())};
    if (letters.size() < 3)
      throw std::invalid_argument("u: the alphabet needs"
                                  " at least 3 letters");
    automaton_t res{ctx};

    // The states.
    std::vector<state_t> states;
    for (unsigned i = 0; i < n; ++i)
      states.push_back(res.new_state());
    res.set_initial(states[0]);
    res.set_final(states[n-1]);

    // The 'a' transitions.
    for (unsigned i = 0; i < n; ++i)
      res.add_transition(states[i], states[(i+1) % n], letters[0]);

    // The 'b' transitions.
    res.add_transition(states[0], states[1], letters[1]);
    res.add_transition(states[1], states[0], letters[1]);
    for (unsigned i = 2; i < n; ++i)
      res.add_transition(states[i], states[i], letters[1]);

    // The 'c' transitions.
    for (unsigned i = 0; i < n - 1; ++i)
      res.add_transition(states[i], states[i], letters[2]);
    res.add_transition(states[n - 1], states[0], letters[2]);

    return res;
  }

  /*---------.
  | dyn::u.  |
  `---------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Ctx>
      automaton
      u(const dyn::context& ctx, unsigned n)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(u<Ctx>(c, n));
      }

      REGISTER_DECLARE(u,
                       (const dyn::context& ctx, unsigned n) -> automaton);
    }
  }

}

#endif // !VCSN_FACTORY_U_HH
