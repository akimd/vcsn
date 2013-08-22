#ifndef VCSN_FACTORY_DOUBLE_RING_HH
# define VCSN_FACTORY_DOUBLE_RING_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>

# include <map>

namespace vcsn
{
  template <class Context>
  mutable_automaton<Context>
  double_ring(unsigned n, const Context& ctx, std::vector<unsigned> finals)
  {
    static_assert(Context::is_lal || Context::is_lan,
                  "requires labels_are_letters or nullable");
    using context_t = Context;
    mutable_automaton<context_t> res{ctx};
    if (n == 0)
      return res;

    // Set initial.
    auto p = res.new_state();
    res.set_initial(p);
    // Have states start on base 0. No need for pre and post states here.
    // FIXME we should use state_t for states.
    std::map<unsigned, unsigned> states;
    // We want first state to be 0 and not 2.
    states.emplace(0, p);
    // Set transitions.
    auto x = p;
    for (unsigned i = 1; i < n; ++i)
      {
        auto y = res.new_state();
        res.add_transition(x, y, 'a');
        res.add_transition(y, x, 'b');
        x = y;
        states.emplace(i, y);
      }
    res.add_transition(x, p, 'a');
    res.add_transition(p, x, 'b');

    // Add finals.
    for (auto f: finals)
    {
      if (f >= n)
        throw std::runtime_error("error: invalid list of finals");
      res.set_final(states[f]);
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
      template <typename Ctx>
      automaton
      double_ring(const dyn::context& ctx, unsigned n, std::vector<unsigned> f)
      {
        const auto& c = dynamic_cast<const Ctx&>(*ctx);
        return make_automaton(c, double_ring<Ctx>(n, c, f));
      }

      REGISTER_DECLARE(double_ring,
                       (const dyn::context& ctx, unsigned n,
                        std::vector<unsigned> f) -> automaton);
    }
  }
}

#endif // !VCSN_FACTORY_DOUBLE_RING_HH
