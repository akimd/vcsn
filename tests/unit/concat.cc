#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/ctx/law_char_z.hh>
#include <vcsn/core/rat/ratexpset.hh>

template <typename Ctx>
static size_t
check_concat(const Ctx& ctx)
{
  size_t nerrs = 0;
  auto ks = vcsn::ratexpset<Ctx>(ctx, vcsn::rat::identities::trivial);

#define CHECK(Lhs, Rhs, Res)                                            \
  ASSERT_EQ(format(ks, ks.concat(conv(ks, Lhs), conv(ks, Rhs))),        \
            Res)

  CHECK("ab.a", "b", "(ab)(ab)");
  CHECK("a", "b.ab", "(ab)(ab)");
  CHECK("ab.a", "b.ab", "(ab)(ab)(ab)");
#undef CHECK
  return nerrs;
}

template <typename Ctx>
static size_t
check_conv(const Ctx& ctx)
{
  size_t nerrs = 0;
  auto ks = vcsn::ratexpset<Ctx>(ctx, vcsn::rat::identities::trivial);
  auto ls = *ctx.labelset();

  // Check that the empty word is really recognized as \e.
  ASSERT_EQ(int(ks.atom(conv(ls, ""))->type()),
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
