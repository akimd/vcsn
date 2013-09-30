#ifndef VCSN_ALGOS_RANDOM_HH
# define VCSN_ALGOS_RANDOM_HH

# include <vcsn/misc/set.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{

  /*--------------------.
  | random(automaton).  |
  `--------------------*/

  template <typename Ctx>
  mutable_automaton<Ctx>
  random(const Ctx& ctx, unsigned num_states)
  {
    using automaton_t = mutable_automaton<Ctx>;
    using state_t = typename automaton_t::state_t;
    automaton_t res(ctx);

    std::vector<state_t> states;
    for (unsigned i = 0; i < num_states; ++i)
      states.push_back(res.new_state());

    return std::move(res);
  }

  namespace dyn
  {
    namespace detail
    {
      /*-------------------.
      | dyn::random(aut).  |
      `-------------------*/
      /// Bridge.
      template <typename Ctx>
      automaton
      random(const context& ctx, unsigned num_states)
      {
        const auto& c = dynamic_cast<const Ctx&>(*ctx);
        return make_automaton(c, random<Ctx>(c, num_states));
      }

      REGISTER_DECLARE(random,
                       (const context& ctx, unsigned n) -> automaton);

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_RANDOM_HH
