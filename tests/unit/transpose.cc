#include <vcsn/algos/transpose.hh>
#include <vcsn/core/rat/ratexpset.hh>
#include <vcsn/ctx/wordset.hh>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <tests/unit/test.hh>


static bool
check_mutable_automaton()
{
  bool res = true;
  // labels_are_letters for weights.
  auto ctx_b = vcsn::ctx::lal_char_b{{'a', 'b', 'c', 'd'}};
  auto ks_b = ctx_b.make_ratexpset();
  // labels_are_words for labels.
  auto ctx_br =
    vcsn::ctx::context<vcsn::ctx::wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                       decltype(ks_b)>
    {{'a', 'b', 'c', 'd'}, ks_b};
  //  auto ks_br = ctx_br.make_ratexpset();

  auto aut1 = vcsn::make_mutable_automaton(ctx_br);

  auto s0 = aut1.new_state();
  auto s1 = aut1.new_state();
  auto s2 = aut1.new_state();

  aut1.set_initial(s0, ks_b.conv("abcd"));
  aut1.add_transition(s0, s0, "cd", ks_b.conv("ab"));
  aut1.add_transition(s0, s1, "cd", ks_b.conv("abcd"));
  aut1.add_transition(s1, s2, "cd");
  aut1.set_initial(s2, ks_b.conv("abcd"));
  std::cout << vcsn::dot(aut1) << '\n';

  auto aut2 = vcsn::transpose(aut1);
  std::cout << vcsn::dot(aut2) << '\n';

# define ASSERT_WEIGHT(Aut, Src, Dst, Lbl, Wgt)                         \
  ASSERT_EQ(Aut.weightset()                                             \
            ->format(Aut.weight_of(Aut.get_transition(Src, Dst, Lbl))), \
            Wgt);

  // Check has_transition and get_transition.
  assert(aut1.has_transition(s0, s1, "cd"));
  // FIXME: we would really like to have equality here on ratexp.
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "a.b.c.d");
  assert(aut2.has_transition(s1, s0, "dc"));
  ASSERT_WEIGHT(aut2, s1, s0, "dc", "d.c.b.a");

  // Now change the transposed automaton, and check the modifications
  // on the original.
  aut2.set_transition(s1, s0, "dc", ks_b.conv("bbaa"));
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "a.a.b.b");
  aut2.del_state(s2);
  aut2.set_initial(s1);

  std::cout << vcsn::dot(aut1) << '\n';
  return res;
}

int main()
{
  unsigned errs = 0;

  errs += !check_mutable_automaton();

  return !!errs;
}
#undef EVAL
