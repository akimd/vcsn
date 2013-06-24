#include <vcsn/algos/transpose.hh>
#include <vcsn/core/rat/ratexpset.hh>
#include <vcsn/ctx/wordset.hh>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <tests/unit/test.hh>


static size_t
check_mutable_automaton()
{
  size_t nerrs = 0;
  // labels_are_letters (w, x, y, z) for weights.
  auto ctx_b = vcsn::ctx::lal_char_b{{'w', 'x', 'y', 'z'}};
  auto ks_b = ctx_b.make_ratexpset();
  // labels_are_words (a, b, c, d) for labels.
  auto ctx_br =
    vcsn::ctx::context<vcsn::ctx::wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                       decltype(ks_b)>
    {{'a', 'b', 'c', 'd'}, ks_b};
  //  auto ks_br = ctx_br.make_ratexpset();

  auto aut1 = vcsn::make_mutable_automaton(ctx_br);

  auto s0 = aut1.new_state();
  auto s1 = aut1.new_state();
  auto s2 = aut1.new_state();

  aut1.set_initial(s0, ks_b.conv("wxyz"));
  aut1.add_transition(s0, s0, "cd", ks_b.conv("wx"));
  aut1.add_transition(s0, s1, "cd", ks_b.conv("wxyz"));
  aut1.add_transition(s1, s2, "cd");
  aut1.set_initial(s2, ks_b.conv("wxyz"));
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
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "w.x.y.z");
  assert(aut2.has_transition(s1, s0, "dc"));
  ASSERT_WEIGHT(aut2, s1, s0, "dc", "z.y.x.w");

  {
    // Wrap a const automaton, and make sure it works properly.
    const auto& caut1 = aut1;
    auto caut2 = vcsn::transpose(caut1);
    ASSERT_EQ(vcsn::dot(caut2), vcsn::dot(aut2));
  }

  // Now change the transposed automaton, and check the modifications
  // on the original.
  aut2.set_transition(s1, s0, "dc", ks_b.conv("yyxx"));
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "x.x.y.y");
  aut2.del_state(s2);
  aut2.set_initial(s1);

  std::cout << vcsn::dot(aut1) << '\n';
  return nerrs;
}

int main()
{
  unsigned errs = 0;

  errs += check_mutable_automaton();

  return !!errs;
}
