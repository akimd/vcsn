#include <vcsn/algos/transpose.hh>
#include <vcsn/core/rat/kratexpset.hh>
#include <vcsn/ctx/char_b_lal.hh>
#include <vcsn/ctx/char_z_lal.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/factory/de_bruijn.hh>
#include <tests/unit/test.hh>

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
  auto ctx_b = vcsn::ctx::char_b_lal{{'a', 'b', 'c', 'd'}};
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

# define ASSERT_WEIGHT(Aut, Src, Dst, Lbl, Wgt)                         \
  ASSERT_EQ(Aut.weightset()                                             \
            ->format(Aut.weight_of(Aut.get_transition(Src, Dst, Lbl))), \
            Wgt);

  // Check has_transition and get_transition.
  assert(aut1.has_transition(s0, s1, "cd"));
  // FIXME: we would really like to have equality here on kratexp.
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "a.b.c.d");
  assert(aut2.has_transition(s1, s0, "dc"));
  ASSERT_WEIGHT(aut2, s1, s0, "dc", "d.c.b.a");

  // Now change the transposed automaton, and check the modifications
  // on the original.
  aut2.set_transition(s1, s0, "dc", ks_b.conv("bbaa"));
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "a.a.b.b");
  aut2.del_state(s2);
  aut2.set_initial(s1);

  vcsn::dotty(aut1, std::cout);
  return res;
}

bool
check_minimization()
{
  using context_t = vcsn::ctx::char_b_lal;
  using automaton_t = vcsn::mutable_automaton<context_t>;
  using tr_automaton_t = vcsn::details::transpose_automaton<automaton_t>;
  context_t ctx{{'a', 'b'}};
  auto ks = ctx.make_kratexpset();
  auto aut = vcsn::standard_of<tr_automaton_t>(ctx, ks.conv("a+a+a+a"));
  auto& au1 = *aut.original_automaton(); //vcsn::dotty(au1, std::cout);
  auto au2 = vcsn::transpose(au1);       //vcsn::dotty(au2, std::cout);
  auto au3 = vcsn::determinize(au2);     //vcsn::dotty(au3, std::cout);
  auto au4 = vcsn::transpose(au3);       //vcsn::dotty(au4, std::cout);
  auto au5 = vcsn::determinize(au4);     vcsn::dotty(au5, std::cout);
  return true;
}

int main()
{
  unsigned errs = 0;

  {
    using context_t = vcsn::ctx::char_z_lal;
    context_t ctx({'a', 'b', 'c'});
    auto aut = vcsn::de_bruijn(2, ctx);
    errs += !check_idempotence(aut);
  }

  errs += !check_mutable_automaton();
  errs += !check_minimization();

  return !!errs;
}
#undef EVAL
