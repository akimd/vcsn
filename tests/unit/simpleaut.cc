#include <vcsn/weights/z.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <iostream>

int main()
{
  typedef vcsn::set_alphabet<vcsn::char_letters> alpha_t;

  alpha_t alpha{'a', 'b', 'c', 'd'};

  //for (auto c : alpha)
  //std::cerr << c << std::endl;

  //  b semiring;

  vcsn::mutable_automaton<alpha_t, vcsn::z,
			  vcsn::labels_are_letters> aut(alpha);

  //mutable_automaton<alpha_t, exp<b, alpha_t> >, labels_are_letter> aut(alpha, semiring);

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

  std::cerr << "Leaving s1 by d" << std::endl;
  for (auto i: aut.out(s1, 'd'))
    {
      std::cerr << aut.dst_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cerr << "Entering s1 by b" << std::endl;
  for (auto i: aut.in(s1, 'b'))
    {
      std::cerr << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cerr << "Between s1 and s1" << std::endl;
  for (auto i: aut.outin(s1, s1))
    {
      std::cerr << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }

  aut.add_transition(s1, s1, 'd', -42);
  auto tj = aut.outin(s1, s1);
  assert(tj.begin() == tj.end());

  aut.del_state(s1);
  vcsn::dotty(aut, std::cout);
  assert(!aut.has_state(s1));
  assert(aut.has_state(s2));
  assert(aut.has_state(s3));

  aut.set_transition(s2, s3, 'a', 0);

  vcsn::dotty(aut, std::cout);
}
