#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/ctx/law_char_z.hh>

using context_t = vcsn::ctx::law_char_z;
using automaton_t = vcsn::mutable_automaton<context_t>;
using entry_t = automaton_t::entry_t;

bool
check_concat(const context_t& ctx)
{
  bool res = true;
  auto ks = ctx.make_ratexpset();

#define CHECK(A, B, C) \
  ASSERT_EQ(ks.format(ks.concat(ks.conv(A), ks.conv(B))), C);

  CHECK("ab.a", "b", "(ab).(ab)");
  CHECK("a", "b.ab", "(ab).(ab)");
  CHECK("ab.a", "b.ab", "(ab).(ab).(ab)");
#undef CHECK
  return res;
}

int main()
{
  size_t nerrs = 0;
  context_t ctx {{'a', 'b'}};
  nerrs += !check_concat(ctx);
  return !!nerrs;
}
