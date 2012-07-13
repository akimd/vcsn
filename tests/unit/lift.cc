#include <iostream>

#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/char_b_lal.hh>
#include <vcsn/factory/ladybird.hh>

using context_t = vcsn::ctx::char_b_lal;
using automaton_t = vcsn::mutable_automaton<context_t>;

bool
check_lift(unsigned size)
{
  context_t ctx{{'a', 'b', 'c'}};
  auto a1 = vcsn::ladybird<context_t>(size, ctx);
  vcsn::dotty(a1, std::cout);
  auto a2 = vcsn::lift<automaton_t>(a1);
  vcsn::dotty(a2, std::cout);
  return true;
}

int main()
{
  bool pass = true;
  pass &= check_lift(4);
  return pass ? 0 : 1;
}
