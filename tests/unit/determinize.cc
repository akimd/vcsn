#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/algos/determinize.hh>

using context_t = vcsn::ctx::lal_char_b;
using automaton_t = vcsn::mutable_automaton<context_t>;

static bool
check_origins(const context_t& ctx)
{
  bool res = true;
  auto nfa = vcsn::de_bruijn(2, ctx);
  vcsn::detail::determinizer<decltype(nfa)> determinize;
  auto dfa = determinize(nfa);
  vcsn::dot(nfa, std::cout) << std::endl;
  vcsn::dot(dfa, std::cout) << std::endl;
  for (auto p : determinize.origins())
    {
      std::cout << "  " << p.first - 2
                << " [label = \"";
      const char* sep = "";
      for (auto s: p.second)
        {
          std::cout << sep << s - 2;
          sep = ",";
        }
      std::cout << "\"]" << std::endl;
    }
  return res;
}

int main()
{
  size_t nerrs = 0;
  context_t ctx {{'a', 'b'}};
  nerrs += !check_origins(ctx);
  return !!nerrs;
}
