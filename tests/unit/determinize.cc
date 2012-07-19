#include <tests/unit/test.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dotty.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/char_b_lal.hh>
#include <vcsn/factory/de_bruijn.hh>
#include <vcsn/factory/ladybird.hh>
#include <tests/unit/test.hh>

using context_t = vcsn::ctx::char_b_lal;
using automaton_t = vcsn::mutable_automaton<context_t>;

/// true iff idempotent.
bool
idempotence(const std::string& str, automaton_t& aut, bool display_aut)
{
  bool res = true;
  auto d1 = vcsn::determinize(aut);
  auto d2 = vcsn::determinize(d1);
  std::cout
    << "Check idempotence for " << str << std::endl
    << "States: " << aut.num_states() << " -> " << d1.num_states() << std::endl;
  if (display_aut)
    vcsn::dotty(d1, std::cout);

  ASSERT_EQ(vcsn::dotty(d1), vcsn::dotty(d2));
  return res;
}

/// true iff passes.
bool
check_de_bruijn(size_t n, bool display_aut)
{
  context_t ctx{{'a', 'b'}};
  automaton_t aut = vcsn::de_bruijn(n, ctx);
  std::ostringstream ss;
  ss << "de Bruijn " << n;
  return idempotence(ss.str(), aut, display_aut);
}


/// true iff passes.
bool
check_ladybird(size_t n, bool display_aut)
{
  context_t ctx{{'a', 'b', 'c'}};
  auto ladybird = vcsn::ladybird<context_t>(n, ctx);
  auto determ_ladybird = vcsn::determinize(ladybird);
  std::ostringstream ss;
  ss << "ladybird " << n;
  return idempotence(ss.str(), ladybird, display_aut);
}


int main()
{
  int errs = 0;
  errs += !check_de_bruijn(3, true);
  errs += !check_de_bruijn(8, false);
  errs += !check_ladybird(4, true);
  errs += !check_ladybird(8, false);
  return !!errs;
}
