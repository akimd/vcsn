#ifndef VCSN_FACTORY_DE_BRUIJN_HH
# define VCSN_FACTORY_DE_BRUIJN_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{
  // (a+b)*a(a+b)^n.
  template <class Context>
  mutable_automaton<Context>
  de_bruijn(unsigned n, const Context& ctx)
  {
    static_assert(Context::is_lal,
                  "requires labels_are_letters");
    using context_t = Context;
    mutable_automaton<context_t> res{ctx};

    auto init = res.new_state();
    res.set_initial(init);
    res.set_transition(init, init, 'a');
    res.set_transition(init, init, 'b');

    auto prev = res.new_state();
    res.set_transition(init, prev, 'a');

    while (n--)
      {
        auto next = res.new_state();
        res.set_transition(prev, next, 'a');
        res.set_transition(prev, next, 'b');
        prev = next;
      }
    res.set_final(prev);
    return res;
  }
}

#endif // !VCSN_FACTORY_DE_BRUIJN_HH
