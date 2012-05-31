#ifndef VCSN_FACTORY_LADYBIRD_HH
# define VCSN_FACTORY_LADYBIRD_HH

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{
  namespace details
  {
    template <class WeightSet>
    struct ladybird_context
    {
      using genset_t = set_alphabet<char_letters>;
      using weightset_t = WeightSet;
    };
  } // namespace details

  template <class WeightSet>
  mutable_automaton<details::ladybird_context<WeightSet>, labels_are_letters>
  ladybird(unsigned n, WeightSet ws = WeightSet())
  {
    // Yes, typedef, not using, as it fails with using.
    typedef typename details::ladybird_context<WeightSet> context_t;
    using genset_t = typename context_t::genset_t;
    static genset_t alpha {'a', 'b', 'c'};
    mutable_automaton<context_t, labels_are_letters> aut(alpha, ws);

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
}

#endif // !VCSN_FACTORY_LADYBIRD_HH
