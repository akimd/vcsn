#include <iostream>
#include <sstream>

#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dotty.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/char.hh>
#include <vcsn/factory/de_bruijn.hh>
#include <vcsn/factory/ladybird.hh>

using context_t = vcsn::ctx::char_b;
using automaton_t = vcsn::mutable_automaton<context_t>;

std::string
to_string(const automaton_t& a)
{
  std::stringstream o;
  vcsn::dotty(a, o);
  return o.str();
}

/// true iff idempotent.
bool
idempotence(std::string str, automaton_t& aut, bool display_aut)
{
  auto d1 = vcsn::determinize(aut);
  auto d2 = vcsn::determinize(d1);
  std::string d1s = to_string(d1);
  std::string d2s = to_string(d2);

  if (d1s == d2s)
    {
      std::cout << "PASS: Check Idempotence for " << str << std::endl;
      if (display_aut)
        std::cout << d1s;
      return true;
    }
  else
    {
      std::cout << "FAIL: determinize(aut) != determinize(determinize(aut)) for "
                << str << std::endl;
      return false;
    }
}

/// true iff passes.
bool
check_simple(size_t n, bool display_aut)
{
  context_t ctx{{'a', 'b'}};
  automaton_t aut = vcsn::de_bruijn(n, ctx);
  std::stringstream ss;
  ss << "de Bruijn " << n;
  return idempotence(ss.str(), aut, display_aut);
}


int main()
{
  int errs = 0;
  errs += !check_simple(3, true);
  errs += !check_simple(8, false);

  context_t ctx{{'a', 'b', 'c'}};
  auto ladybird = vcsn::ladybird<context_t>(4, ctx);
  auto determ_ladybird = vcsn::determinize(ladybird);

  errs += !idempotence("ladybird 4", ladybird, true);
  return !!errs;
}
