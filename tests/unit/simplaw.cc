#include <iostream>

#include <vcsn/algos/dotty.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/char.hh>

int main()
{
  using context_t = vcsn::ctx::char_z;
  context_t ctx {.gs_ = {'a', 'b', 'c', 'd'}, .ws_ = {}};
  using automaton_t =
    vcsn::mutable_automaton<context_t, vcsn::labels_are_words>;
  automaton_t aut(ctx);

  auto s1 = aut.new_state();
  auto s2 = aut.new_state();
  auto s3 = aut.new_state();

  aut.set_initial(s1);
  aut.set_final(s2, 10);
  aut.set_transition(s1, s2, "ca", 42);
  aut.set_transition(s2, s3, "a", 1);
  aut.set_transition(s2, s1, "ba", 1);
  aut.set_transition(s3, s2, "", 2);

  vcsn::dotty(aut, std::cout);
}
