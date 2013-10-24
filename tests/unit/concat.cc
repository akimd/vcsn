#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/ctx/law_char_z.hh>

template <typename Ctx>
static size_t
check_concat(const Ctx& ctx)
{
  size_t nerrs = 0;
  auto ks = ctx.make_ratexpset();

#define CHECK(Lhs, Rhs, Res)                                    \
  ASSERT_EQ(ks.format(ks.concat(ks.conv(Lhs), ks.conv(Rhs))),   \
            Res)

  CHECK("ab.a", "b", "ab.ab");
  CHECK("a", "b.ab", "ab.ab");
  CHECK("ab.a", "b.ab", "ab.ab.ab");
#undef CHECK
  return nerrs;
}

template <typename Ctx>
static size_t
check_conv(const Ctx& ctx)
{
  size_t nerrs = 0;
  auto ks = ctx.make_ratexpset();
  auto ls = *ctx.labelset();

  // Check that the empty word is really recognized as \e.
  ASSERT_EQ(int(ks.atom(ls.conv(""))->type()),
            int(vcsn::rat::exp::type_t::one));

  return nerrs;
}

int main()
{
  size_t nerrs = 0;
  nerrs += check_concat(vcsn::ctx::law_char_z{{'a', 'b'}});
  nerrs += check_conv(vcsn::ctx::law_char_z{{'a', 'b'}});
  return !!nerrs;
}
