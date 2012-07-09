#include <iostream>

#include <vcsn/misc/echo.hh>
#include <vcsn/algos/dotty.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/ctx/char.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/factory/de_bruijn.hh>

#define ASSERT_EQ(Lhs, Rhs)                                     \
  do {                                                          \
    auto lhs = Lhs;                                             \
    auto rhs = Rhs;                                             \
    if (lhs != rhs)                                             \
      {                                                         \
        ECHOH("assertion failed: " #Lhs " == " #Rhs);           \
        ECHOH("    " << lhs << " != " << rhs);                  \
        res = false;                                            \
      }                                                         \
  } while (false)

template <typename Aut>
bool
check_idempotence(Aut& aut)
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
  ASSERT_EQ (s, s2);
  return res;
}

bool
check_mutable_automaton()
{
  bool res = true;
  // labels_are_letters for weights.
  auto ctx_b =
    vcsn::ctx::char_<vcsn::b, vcsn::labels_are_letters>{{'a', 'b', 'c', 'd'}};
  auto ks_b = ctx_b.make_kratexpset();
  // labels_are_words for labels.
  auto ctx_br =
    vcsn::ctx::char_<decltype(ks_b), vcsn::labels_are_words>
    {{'a', 'b', 'c', 'd'}, ks_b};
  //  auto ks_br = ctx_br.make_kratexpset();

  auto aut1 = vcsn::make_mutable_automaton(ctx_br);

  auto s0 = aut1.new_state();
  auto s1 = aut1.new_state();
  auto s2 = aut1.new_state();

  aut1.set_initial(s0, ks_b.conv("abcd"));
  aut1.add_transition(s0, s0, "cd", ks_b.conv("ab"));
  aut1.add_transition(s0, s1, "cd", ks_b.conv("abcd"));
  aut1.add_transition(s1, s2, "cd");
  aut1.set_initial(s2, ks_b.conv("abcd"));
  vcsn::dotty(aut1, std::cout);

  auto aut2 = vcsn::transpose(aut1);
  vcsn::dotty(aut2, std::cout);

  // Check has_transition and get_transition.
  assert(aut1.has_transition(s0, s1, "cd"));
  // FIXME: we would really like to have equality here on kratexp.
  ASSERT_EQ(aut1.weightset()
            ->format(aut1.weight_of(aut1.get_transition(s0, s1, "cd"))),
            "a.b.c.d");
  assert(aut2.has_transition(s1, s0, "dc"));
  ASSERT_EQ(aut2.weightset()
            ->format(aut2.weight_of(aut2.get_transition(s1, s0, "dc"))),
            "d.c.b.a");

  return res;
}

int main()
{
  unsigned errs = 0;

  {
    using context_t = vcsn::ctx::char_z;
    context_t ctx({'a', 'b', 'c'});
    auto aut = vcsn::de_bruijn(2, ctx);
    errs += !check_idempotence(aut);
  }

  {
    errs += !check_mutable_automaton();
  }

  return !!errs;
}
#undef EVAL
