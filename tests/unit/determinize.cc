#include <iostream>
#include <sstream>

#include <vcsn/weights/b.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dotty.hh>
#include <vcsn/factory/ladybird.hh>

typedef vcsn::set_alphabet<vcsn::char_letters> alpha_t;
typedef vcsn::mutable_automaton<alpha_t,
                                vcsn::b,
                                vcsn::labels_are_letters> automaton_t;

automaton_t factory(int n, alpha_t& alpha, vcsn::b& b)
{
  assert(n > 1);

  automaton_t res {alpha, b};

  auto init = res.new_state();
  res.set_initial(init);
  res.set_transition(init, init, 'a');
  res.set_transition(init, init, 'b');

  auto prev = res.new_state();

  res.set_transition(init, prev, 'a');

  --n;

  while (--n)
    {
      auto next = res.new_state();
      res.set_transition(prev, next, 'a');
      res.set_transition(prev, next, 'b');
      prev = next;
    }
  res.set_final(prev);
  return res;
}

bool check_idempotence(size_t n, bool display_aut)
{
  alpha_t alpha {'a', 'b'};
  vcsn::b b;

  automaton_t aut = factory(n, alpha, b);

  auto determ = vcsn::determinize(aut);
  auto id = vcsn::determinize(determ);
  std::stringstream determ_string;
  std::stringstream id_string;
  vcsn::dotty(determ, determ_string);
  vcsn::dotty(id, id_string);
  if (!determ_string.str().compare(id_string.str()))
    {
      std::cout <<"PASS: Check Idempotence for: " << n << std::endl;
      if (display_aut)
        std::cout << determ_string.str();
      return true;
    }
  else
    {
      std::cout << "FAIL: determinize(aut) != determinize(determinize(aut)) for size: "
                << n << std::endl;
      return false;
    }
}

int main()
{
  vcsn::b b;

  check_idempotence(5, true);
  check_idempotence(10, false);
  // FIXME: This test segfault due the genset.
  // auto ladybird = vcsn::ladybird<vcsn::b>(4, b);

  // auto determ_ladybird = vcsn::determinize(ladybird);

  // vcsn::dotty(determ_ladybird, std::cout);
}
