#ifndef VCSN_FACTORY_LADYBIRD_HH
#define VCSN_FACTORY_LADYBIRD_HH

#include "vcsn/alphabets/char.hh"
#include "vcsn/alphabets/setalpha.hh"
#include "vcsn/core/mutable_automaton.hh"

namespace vcsn
{
  template<class Weights>
  mutable_automaton<set_alphabet<char_letters>, Weights, labels_are_letters>
  ladybird(unsigned n, Weights w = Weights())
  {
    set_alphabet<char_letters> alpha;
    alpha.add_letter('a').add_letter('b').add_letter('c');
    mutable_automaton<set_alphabet<char_letters>,
		      Weights, labels_are_letters> aut(alpha, w);

    auto p = aut.add_state();
    aut.set_initial(p);
    aut.set_final(p);
    auto x = p;
    for(unsigned i = 1; i < n; ++i)
      {
	auto y = aut.add_state();
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

#endif // VCSN_FACTORY_LADYBIRD_HH
