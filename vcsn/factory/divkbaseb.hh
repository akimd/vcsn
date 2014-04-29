#ifndef VCSN_FACTORY_DIVKBASEB_HH
# define VCSN_FACTORY_DIVKBASEB_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  /// Build the Boolean automaton which accepts a word n representing a
  /// number in base "base" if and only if divisor|n.
  template <typename Context>
  mutable_automaton<Context>
  divkbaseb(const Context& ctx, unsigned divisor, unsigned base)
  {
    using context_t = Context;
    using automaton_t = mutable_automaton<context_t>;
    using state_t = typename automaton_t::state_t;
    const auto& gens = ctx.labelset()->genset();
    std::vector<typename context_t::labelset_t::letter_t> letters
      {std::begin(gens), std::end(gens)};

    require(divisor,
            "divkbaseb: divisor cannot be 0");
    require(2 <= base,
            "divkbaseb: base (" + std::to_string(base)
            + ") must be at least 2");
    require(base <= letters.size(),
            "divkbaseb: base (" + std::to_string(base)
            + ") must be less than or equal to the alphabet size ("
            + std::to_string(letters.size()) + ")");

    automaton_t res{ctx};

    // Add one state for each possible remainder. The last state encountered
    // during the evaluation will be n % k. If the last state is the state 0,
    // it means that the residue is 0, ie the word will be accepted, ie the
    // number is a multiple of k.
    std::vector<state_t> states;
    for (unsigned i = 0; i < divisor; ++i)
      states.emplace_back(res.new_state());

    res.set_initial(states[0]);
    res.set_final(states[0]);

    for (unsigned i = 0; i < divisor; ++i)
      {
        int e = i * base;
        for (unsigned l = 0; l < base; ++l)
          {
            int d = (e + l) % divisor;
            res.new_transition(states[i], states[d], letters[l]);
          }
      }
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename Unsigned1, typename Unsigned2>
      automaton
      divkbaseb(const context& ctx, unsigned divisor, unsigned base)
      {
        const auto& c = ctx->as<Ctx>();
        return make_automaton(divkbaseb<Ctx>(c, divisor, base));
      }

      REGISTER_DECLARE
      (divkbaseb,
       (const context& ctx, unsigned k, unsigned b) -> automaton);
    }
  }

}

#endif // !VCSN_FACTORY_DIVKBASEB_HH
