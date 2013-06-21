#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/algos/dot.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/lal_char_z.hh>

using context_t = vcsn::ctx::lal_char_z;
using automaton_t = vcsn::mutable_automaton<context_t>;

template <typename Aut>
std::vector<typename Aut::state_t>
new_states(Aut& aut, size_t n)
{
  std::vector<typename Aut::state_t> res;
  res.reserve(n);
  for (size_t i = 0; i < n; ++i)
    res.push_back(aut.new_state());
  return std::move(res);
}

template <typename Aut>
Aut
clique(const typename Aut::context_t& ctx, size_t size)
{
  Aut res{ctx};
  const auto& letters = *ctx.labelset();
  auto ss = new_states(res, size);
  for (auto s: ss)
    for (auto d: ss)
      for (auto l: letters)
        res.add_transition(s, d, l);
  return std::move(res);
}

static size_t
check_various(const context_t& ctx)
{
  size_t nerrs = 0;
  automaton_t aut{ctx};

  auto s1 = aut.new_state();
  auto s2 = aut.new_state();
  auto s3 = aut.new_state();
  aut.set_initial(s1);
  aut.set_final(s2, 10);
  aut.set_transition(s1, s2, 'c', 42);
  aut.set_transition(s2, s3, 'a', 1);
  aut.set_transition(s2, s1, 'b', 1);
  int v = aut.add_weight(aut.set_transition(s1, s1, 'd', 2), 40);
  ASSERT_EQ(v, 42);
  aut.set_transition(s1, s3, 'd', 1);
  std::cout << vcsn::dot(aut) << '\n';
  ASSERT_EQ(aut.num_states(), 3u);
  ASSERT_EQ(aut.num_transitions(), 5u);

  std::cout << "Leaving s1 by d" << std::endl;
  for (auto i: aut.out(s1, 'd'))
    {
      std::cout << i << " " << aut.dst_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cout << "Entering s1 by b" << std::endl;
  for (auto i: aut.in(s1, 'b'))
    {
      std::cout << i << " " << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cout << "Between s1 and s1" << std::endl;
  for (auto i: aut.outin(s1, s1))
    {
      std::cout << i << " " << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }

  aut.add_transition(s1, s1, 'd', -42);
  std::cout << vcsn::dot(aut) << '\n';
  auto tj = aut.outin(s1, s1);
  assert(tj.begin() == tj.end());
  ASSERT_EQ(aut.num_states(), 3u);
  ASSERT_EQ(aut.num_transitions(), 4u);

  aut.del_state(s1);
  std::cout << vcsn::dot(aut) << '\n';
  assert(!aut.has_state(s1));
  assert(aut.has_state(s2));
  assert(aut.has_state(s3));

  ASSERT_EQ(aut.num_states(), 2u);
  ASSERT_EQ(aut.num_transitions(), 1u);

  aut.set_transition(s2, s3, 'a', 0);

  std::cout << vcsn::dot(aut) << '\n';
  ASSERT_EQ(aut.num_states(), 2u);
  ASSERT_EQ(aut.num_transitions(), 0u);
  return nerrs;
}

static size_t
check_del_transition(const context_t& ctx)
{
  size_t nerrs = 0;
  automaton_t aut = clique<automaton_t>(ctx, 3);
  ASSERT_EQ(aut.num_transitions(), 3 * 3 * 4u); // 4 letters.
  const auto& ss = aut.states();
  std::vector<automaton_t::state_t> s{std::begin(ss), std::end(ss)};

  aut.del_transition(s[0], s[1]);
  ASSERT_EQ(aut.num_transitions(), (3 * 3 - 1) * 4u);
  aut.del_transition(s[0], s[1]);
  ASSERT_EQ(aut.num_transitions(), (3 * 3 - 1) * 4u);

  aut.del_transition(s[2], s[2]);
  ASSERT_EQ(aut.num_transitions(), (3 * 3 - 2) * 4u);

  return nerrs;
}

int main()
{
  size_t nerrs = 0;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  nerrs += check_various(ctx);
  nerrs += check_del_transition(ctx);
  return !!nerrs;
}
