#include <iostream>

#include <vcsn/algos/dot.hh>
#include <vcsn/algos/product.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/core/mutable_automaton.hh>

int main()
{
  using context_t = vcsn::ctx::lal_char_z;
  context_t ctx { {'a', 'b', 'c'} };
  using automaton_t = vcsn::mutable_automaton<context_t>;

  // {2}(a*b*a*)
  automaton_t aut1(ctx);
  auto s1 = aut1.new_state();
  auto s2 = aut1.new_state();
  auto s3 = aut1.new_state();
  aut1.set_initial(s1, 2);
  aut1.set_final(s1);
  aut1.set_final(s2);
  aut1.set_final(s3);
  aut1.set_transition(s1, s1, 'a');
  aut1.set_transition(s1, s2, 'b');
  aut1.set_transition(s1, s3, 'a');
  aut1.set_transition(s2, s2, 'b');
  aut1.set_transition(s2, s3, 'a');
  aut1.set_transition(s3, s3, 'a');

  // ({3}(ab))*
  automaton_t aut2(ctx);
  auto t1 = aut2.new_state();
  auto t2 = aut2.new_state();
  aut2.set_initial(t1, 3);
  aut2.set_final(t1);
  aut2.set_transition(t1, t2, 'a');
  aut2.set_transition(t2, t1, 'b', 3);

  std::cout << vcsn::dot(aut1) << '\n';
  std::cout << vcsn::dot(aut2) << '\n';

  automaton_t prod = product(aut1, aut2);

  std::cout << vcsn::dot(prod) << '\n';
}
