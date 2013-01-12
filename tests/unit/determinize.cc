#include <tests/unit/test.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dot.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/factory/de_bruijn.hh>
#include <vcsn/factory/ladybird.hh>
#include <tests/unit/test.hh>

using context_t = vcsn::ctx::lal_char_b;
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
    std::cout << vcsn::dot(d1) << '\n';

  ASSERT_EQ(vcsn::dot(d1), vcsn::dot(d2));
  return res;
}

/// true iff passes.
bool
check_ladybird(size_t n, bool display_aut)
{
  context_t ctx{{'a', 'b', 'c'}};
  auto ladybird = vcsn::ladybird<context_t>(n, ctx);
  std::ostringstream ss;
  ss << "ladybird " << n;
  return idempotence(ss.str(), ladybird, display_aut);
}

int main()
{
  int errs = 0;
  errs += !check_ladybird(4, true);
  errs += !check_ladybird(8, false);
  return !!errs;
}
