#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/algos/determinize.hh>

using context_t = vcsn::ctx::lal_char_b;
using automaton_t = vcsn::mutable_automaton<context_t>;

template <typename Aut>
static bool
check_origins(const std::string& title, const Aut& nfa)
{
  bool res = true;
  std::cout << title << std::endl;
  vcsn::detail::determinizer<Aut> determinize;
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
  nerrs += !check_origins("de bruijn 2", vcsn::de_bruijn(2, ctx));
  {
    // Also check the introduction of the sink state.
    automaton_t aut{ctx};
    auto si = aut.new_state();
    auto s0 = aut.new_state();
    auto s1 = aut.new_state();
    auto s2 = aut.new_state();
    aut.set_initial(si);
    aut.set_final(s0);
    aut.set_final(s1);
    aut.set_final(s2);
    aut.set_transition(si, s0, 'a');
    aut.set_transition(si, s1, 'a');
    aut.set_transition(si, s2, 'a');
    check_origins("simple", aut);
  }
  return !!nerrs;
}
