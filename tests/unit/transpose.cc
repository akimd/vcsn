#undef NDEBUG

#include <vcsn/algos/transpose.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/labelset/wordset.hh>

// Include this one last, as it defines a macro `V`, which is used as
// a template parameter in boost/unordered/detail/allocate.hpp.
#include "tests/unit/test.hh"

static size_t
check_mutable_automaton()
{
  size_t nerrs = 0;

  // labels_are_letters (w, x, y, z) for weights.
  auto ctx_b = vcsn::ctx::lal_char_b{{'w', 'x', 'y', 'z'}};
  auto ks_b = vcsn::expressionset<vcsn::ctx::lal_char_b>(ctx_b);
  // At some point, because of improper implementation of conv, we
  // used to fail a cast here.
  conv(ks_b, "wxyz");

  // labels_are_words (a, b, c, d) for labels.
  auto ctx_br =
    vcsn::context<vcsn::wordset<vcsn::set_alphabet<vcsn::char_letters>>,
                  decltype(ks_b)>
    {{'a', 'b', 'c', 'd'}, ks_b};
  //  auto ks_br = ctx_br.make_expressionset();

  auto aut1 = vcsn::make_mutable_automaton(ctx_br);

  auto s0 = aut1->new_state();
  auto s1 = aut1->new_state();
  auto s2 = aut1->new_state();

  aut1->set_initial(s0, conv(ks_b, "wxyz"));
  aut1->new_transition(s0, s0, "cd", conv(ks_b, "wx"));
  aut1->new_transition(s0, s1, "cd", conv(ks_b, "wxyz"));
  aut1->new_transition(s1, s2, "cd");
  aut1->set_initial(s2, conv(ks_b, "wxyz"));
  vcsn::dot(aut1, std::cout) << '\n';

  auto aut2 = vcsn::transpose(aut1);
  vcsn::dot(aut2, std::cout) << '\n';

# define ASSERT_WEIGHT(Aut, Src, Dst, Lbl, Wgt)                         \
  ASSERT_EQ(to_string(*Aut->weightset(),                                \
                      Aut->weight_of(Aut->get_transition(Src, Dst, Lbl))), \
            Wgt);

  // Check has_transition and get_transition.
  assert(aut1->has_transition(s0, s1, "cd"));
  // FIXME: we would really like to have equality here on expression.
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "wxyz");
  assert(aut2->has_transition(s1, s0, "dc"));
  ASSERT_WEIGHT(aut2, s1, s0, "dc", "zyxw");

  {
    // Wrap a const automaton, and make sure it works properly.
    const auto& caut1 = aut1;
    auto caut2 = vcsn::transpose(caut1);
    std::ostringstream os;
    vcsn::dot(caut2, os) << std::endl;
    std::string s1 = os.str();
    os.str("");
    vcsn::dot(aut2, os) << std::endl;
    std::string s2 = os.str();
    ASSERT_EQ(s1, s2);
  }

  // Now change the transposed automaton, and check the modifications
  // on the original.
  aut2->set_transition(s1, s0, "dc", conv(ks_b, "yyxx"));
  ASSERT_WEIGHT(aut1, s0, s1, "cd", "xxyy");
  aut2->del_state(s2);
  aut2->set_initial(s1);

  vcsn::dot(aut1, std::cout) << '\n';
  return nerrs;
}

int main()
{
  unsigned errs = 0;

  errs += check_mutable_automaton();

  return !!errs;
}
