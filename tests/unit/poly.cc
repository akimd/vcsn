#include <cassert>
#include <vcsn/ctx/char_z_lal.hh>
#include <vcsn/weights/poly.hh>
#include <tests/unit/test.hh>

int main()
{
  bool res = true;
  using context_t = vcsn::ctx::char_z_lal;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  using poly_t = vcsn::polynomials<context_t>;
  poly_t poly{ctx};

  poly_t::value_t u = poly.unit();
  poly.assoc(u, "ab", 12);
  ASSERT_EQ(poly.format(u), "\\e + {12}ab");

  u = poly.add(u, u);
  ASSERT_EQ(poly.format(u), "{2}\\e + {24}ab");

  assert(!poly.is_zero(u));
  assert(poly.is_zero(poly.zero()));

  poly_t::value_t v;
  poly.assoc(v, "ab", 24);
  poly.assoc(v, "", 2);
  assert(u == v);

  u = poly.mul(u, u);
  ASSERT_EQ(poly.format(u), "{4}\\e + {96}ab + {576}abab");

  poly.add_assoc(v, "ab", 96 - 24);
  poly.assoc(v, "", 4);
  poly.assoc(v, "abab", 576);
  assert(u == v);

  return !res;
}
