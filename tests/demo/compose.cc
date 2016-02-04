#include <iostream> //std::cout

#include <vcsn/alphabets/char.hh> // type of letter
#include <vcsn/alphabets/setalpha.hh> // alphabet
#include <vcsn/algos/compose.hh> // composition algorithm
#include <vcsn/algos/dot.hh> // display algorithm
#include <vcsn/labelset/tupleset.hh> // tupleset
#include <vcsn/labelset/letterset.hh> // letterset
#include <vcsn/weightset/b.hh> // weightset b

/**
 * Example usage of Vcsn's static layer.
 *
 * Creation of an automaton, composition, then display.
 */
int main(int argc, const char *argv[])
{
  using namespace vcsn;

  /// Alphabet
  using alphabet_t = set_alphabet<char_letters>;

  /// Letterset (labelset with a simple alphabet)
  using letterset_t = letterset<alphabet_t>;

  /// Tupleset (two-tape labelset)
  using tupleset_t = tupleset<letterset_t, letterset_t>;

  /// Context of the automaton: lat<lal_char, lal_char>, b
  using context_t = context<tupleset_t, b>;

  /// Type of the automaton
  using automaton_t = mutable_automaton<context_t>;

  /// Create the letterset
  // It's open, we can add letters to the alphabet
  auto al1 = letterset_t();

  /// Create the labelset (tupleset)
  auto tp = tupleset_t{{al1}, {al1}};

  /// Create the context from the labelset
  // no parameter for the weightset, as it's just B
  context_t ctx1 {tp};

  /// Create an empty automaton, from the context
  auto l = make_mutable_automaton(ctx1);

  /// Add a state.
  // p is a state identifier, probably 0
  auto p = l->new_state();
  auto q = l->new_state();

  /// Make p initial
  l->set_initial(p);
  /// Transition from p to q, with label "a|a"
  l->new_transition(p, q, ctx1.labelset()->tuple('a', 'a'));
  /// Make q final
  l->set_final(q);

  // The automaton looks like:
  // --> p --(a|a)--> q -->

  /// Lazy composition with itself
  auto c = compose_lazy(l, l);

  /// Display the automaton on standard output
  // It's not complete, because it's a lazy composition
  dot(c, std::cout);

  /// Compute the accessible states of c, thus resolving all the states
  auto d = accessible(c);

  /// Display d
  dot(d, std::cout);
  /// Display c, all the states have been resolved
  dot(c, std::cout);
  return 0;
}
