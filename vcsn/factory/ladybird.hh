#ifndef VCSN_FACTORY_LADYBIRD_HH
# define VCSN_FACTORY_LADYBIRD_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{
  template <class Context>
  mutable_automaton<Context, labels_are_letters>
  ladybird(unsigned n, const Context& ctx)
  {
    using context_t = Context;
    mutable_automaton<context_t, labels_are_letters> aut(ctx);

    auto p = aut.new_state();
    aut.set_initial(p);
    aut.set_final(p);
    auto x = p;
    for (unsigned i = 1; i < n; ++i)
      {
	auto y = aut.new_state();
	aut.add_transition(x, y, 'a');
	aut.add_transition(y, y, 'b');
	aut.add_transition(y, y, 'c');
	aut.add_transition(y, p, 'c');
	x = y;
      }
    aut.add_transition(x, p, 'a');
    return aut;
  }

  template <class Context>
  inline
  mutable_automaton<Context, labels_are_letters>
  make_ladybird(unsigned n, const Context& ctx)
  {
    return ladybird(n, ctx);
  }
}

#endif // !VCSN_FACTORY_LADYBIRD_HH
