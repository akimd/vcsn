#include <iostream>

#include <vcsn/algos/dotty.hh>
#include <vcsn/algos/product.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/weights/z.hh>

int main()
{
  using alpha_t = vcsn::set_alphabet<vcsn::char_letters>;
  struct context_t
  {
    using genset_t = alpha_t;
    using weightset_t = vcsn::z;
  };
  using  automaton_t =
    vcsn::mutable_automaton<context_t, vcsn::labels_are_letters>;
  vcsn::z z;
  alpha_t alpha{'a', 'b', 'c'};



  // {2}(a*b*a*)
  automaton_t aut1(alpha, z);
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
  automaton_t aut2(alpha, z);
  auto t1 = aut2.new_state();
  auto t2 = aut2.new_state();
  aut2.set_initial(t1, 3);
  aut2.set_final(t1);
  aut2.set_transition(t1, t2, 'a');
  aut2.set_transition(t2, t1, 'b', 3);

  vcsn::dotty(aut1, std::cout);
  vcsn::dotty(aut2, std::cout);

  automaton_t prod = product(aut1, aut2);

  vcsn::dotty(prod, std::cout);
}
