#include <iostream>

#include <vcsn/ctx/law_char_z.hh>
#include <vcsn/core/rat/expressionset.hh>

// Include this one last, as it defines a macro `V`, which is used as
// a template parameter in boost/unordered/detail/allocate.hpp.
#include "tests/unit/test.hh"

template <typename Ctx>
static size_t
check_concat(const Ctx& ctx)
{
  size_t nerrs = 0;
  auto ks = vcsn::expressionset<Ctx>(ctx);

#define CHECK(Lhs, Rhs, Res)                                            \
  ASSERT_EQ(Res,                                                        \
            to_string(ks, ks.concat(conv(ks, Lhs), conv(ks, Rhs))))

  CHECK("ab.a", "b", "(ab)²");
  CHECK("a", "b.ab", "(ab)²");
  CHECK("ab.a", "b.ab", "(ab)³");
#undef CHECK
  return nerrs;
}

template <typename Ctx>
static size_t
check_conv(const Ctx& ctx)
{
  size_t nerrs = 0;
  auto ks = vcsn::expressionset<Ctx>(ctx);
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
