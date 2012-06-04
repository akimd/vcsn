#include <iostream>
#include <vcsn/algos/dotty.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/char.hh>

int main()
{
  using context_t = vcsn::ctx::char_z;
  context_t ctx {.gs_ = {'a', 'b', 'c', 'd'}, .ws_ = {}};

  using automaton_t =
    vcsn::mutable_automaton<context_t, vcsn::labels_are_letters>;
  automaton_t aut{ctx};

  auto s1 = aut.new_state();
  auto s2 = aut.new_state();
  auto s3 = aut.new_state();
  aut.set_initial(s1);
  aut.set_final(s2, 10);
  aut.set_transition(s1, s2, 'c', 42);
  aut.set_transition(s2, s3, 'a', 1);
  aut.set_transition(s2, s1, 'b', 1);
  int v = aut.add_weight(aut.set_transition(s1, s1, 'd', 2), 40);
  assert(v == 42);
  aut.set_transition(s1, s3, 'd', 1);
  vcsn::dotty(aut, std::cout);
  assert(aut.num_states() == 3);
  assert(aut.num_transitions() == 5);

  std::cout << "Leaving s1 by d" << std::endl;
  for (auto i: aut.out(s1, 'd'))
    {
      std::cout << i << " " << aut.dst_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cout << "Entering s1 by b" << std::endl;
  for (auto i: aut.in(s1, 'b'))
    {
      std::cout << i << " " << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cout << "Between s1 and s1" << std::endl;
  for (auto i: aut.outin(s1, s1))
    {
      std::cout << i << " " << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }

  aut.add_transition(s1, s1, 'd', -42);
  vcsn::dotty(aut, std::cout);
  auto tj = aut.outin(s1, s1);
  assert(tj.begin() == tj.end());
  assert(aut.num_states() == 3);
  assert(aut.num_transitions() == 4);

  aut.del_state(s1);
  vcsn::dotty(aut, std::cout);
  assert(!aut.has_state(s1));
  assert(aut.has_state(s2));
  assert(aut.has_state(s3));

  assert(aut.num_states() == 2);
  assert(aut.num_transitions() == 1);

  aut.set_transition(s2, s3, 'a', 0);

  vcsn::dotty(aut, std::cout);
  assert(aut.num_states() == 2);
  assert(aut.num_transitions() == 0);
}
