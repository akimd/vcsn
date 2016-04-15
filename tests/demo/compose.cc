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
int main()
{
  using namespace vcsn;

  // Basic alphabet type.
  using alphabet_t = set_alphabet<char_letters>;

  // Letterset (single-tape labelset).
  using letterset_t = letterset<alphabet_t>;

  // Create the letterset.
  auto ls1 = letterset_t{'a'};

  // Labelset (double-tape).
  using labelset_t = tupleset<letterset_t, letterset_t>;

  // Create the double-tape labelset.
  auto ls = labelset_t{ls1, ls1};


  // Context of the automaton: lat<lal_char, lal_char>, b.
  using context_t = context<labelset_t, b>;

  // Create the context from the labelset.
  // No parameter for the weightset, as it's just B.
  auto ctx = context_t{ls};


  // Create an empty automaton (transducer), from the context.
  auto t = make_mutable_automaton(ctx);

  // Add a state.
  // p is a state identifier, probably 0
  auto p = t->new_state();
  auto q = t->new_state();

  // Make p initial.
  t->set_initial(p);
  // Transition from p to q, with label "a|a".
  t->new_transition(p, q, ctx.labelset()->tuple('a', 'a'));
  // Make q final
  t->set_final(q);

  // The automaton looks like:
  // --> p --(a|a)--> q -->

  /// Lazy composition with itself
  auto c = compose_lazy(t, t);

  // Display the resulting automaton on standard output.
  // It's partial, because it's a lazy composition.
  dot(c, std::cout) << '\n';

  // Compute the accessible states of c, thus resolving all the states.
  auto d = accessible(c);

  // Display d.
  dot(d, std::cout) << '\n';
  // Display c, all the states have been resolved.
  dot(c, std::cout) << '\n';
}
