#include <iostream>

#include <vcsn/algos/dot.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/law_char_z.hh>

int main()
{
  using context_t = vcsn::ctx::law_char_z;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  using automaton_t = vcsn::mutable_automaton<context_t>;
  automaton_t aut = vcsn::make_shared_ptr<automaton_t>(ctx);

  auto s1 = aut->new_state();
  auto s2 = aut->new_state();
  auto s3 = aut->new_state();

  aut->set_initial(s1);
  aut->set_final(s2, 10);
  aut->set_transition(s1, s2, "ca", 42);
  aut->set_transition(s2, s3, "a", 1);
  aut->set_transition(s2, s1, "ba", 1);
  aut->set_transition(s3, s2, "", 2);

  vcsn::dot(aut, std::cout) << '\n';
}
