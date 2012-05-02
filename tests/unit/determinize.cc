#include <iostream>

#include <vcsn/weights/b.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dotty.hh>


typedef vcsn::set_alphabet<vcsn::char_letters> alpha_t;
typedef vcsn::mutable_automaton<alpha_t, vcsn::b, vcsn::labels_are_letters> automaton_t;

automaton_t *factory(int n, alpha_t& alpha, vcsn::b& b)
{
  if (n < 1)
    return nullptr;
  automaton_t *aut = new automaton_t(alpha, b);

  auto init = aut->new_state();
  aut->set_initial(init);
  aut->set_transition(init, init, 'a');
  aut->set_transition(init, init, 'b');

  auto prev = aut->new_state();

  aut->set_transition(init, prev, 'a');

  --n;

  while (--n)
    {
      auto next = aut->new_state();
      aut->set_transition(prev, next, 'a');
      aut->set_transition(prev, next, 'b');
      prev = next;
    }
  aut->set_final(prev);
  return aut;
}

int main()
{
  alpha_t alpha {'a', 'b'};
  vcsn::b b;

  automaton_t *aut = factory(5, alpha, b);
  if (aut != nullptr)
    {
      auto res = vcsn::determinize(*aut);
      vcsn::dotty(res, std::cout);
    }
  delete aut;
}
