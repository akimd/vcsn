#include <iostream>

#include <vcsn/algos/dotty.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/ctx/char.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/factory/de_bruijn.hh>

template <typename Aut>
bool
check(Aut& aut)
{
  bool res = true;
  std::string s = vcsn::dotty(aut);
  auto t1 = vcsn::transpose(aut);
  auto s1 = vcsn::dotty(t1);
  if (s == s1)
    {
      res = false;
      std::cerr << "s == t1: " << s1 << std::endl;
    }

  auto t2 = vcsn::transpose(t1);
  t2.states();
  auto s2 = vcsn::dotty(t2);
  if (s != s2)
    {
      res = false;
      std::cerr << "s != s2" << std::endl;
      std::cerr << "s: " << s << std::endl;
      std::cerr << "s2: " << s2 << std::endl;
    }
  return res;
}

int main()
{
  unsigned errs = 0;

  {
    using context_t = vcsn::ctx::char_z;
    context_t ctx({'a', 'b', 'c'});
    auto aut = vcsn::de_bruijn(2, ctx);
    errs += !check(aut);
  }

  return !!errs;
}
#undef EVAL
