#include <cassert>
#include <vcsn/ctx/char.hh>
#include <vcsn/weights/poly.hh>

int main()
{
  using context_t = vcsn::ctx::char_z;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  using poly_t = vcsn::polynomials<context_t>;
  poly_t poly(ctx);

  poly_t::value_t u = poly.unit();
  poly.assoc(u, "ab", 12);
  poly.print(std::cout, u) << std::endl;

  u = poly.add(u, u);
  poly.print(std::cout, u) << std::endl;

  assert(!poly.is_zero(u));
  assert(poly.is_zero(poly.zero()));

  poly_t::value_t v;
  poly.assoc(v, "ab", 24);
  poly.assoc(v, "", 2);
  assert(u == v);

  u = poly.mul(u, u);
  poly.print(std::cout, u) << std::endl;

  poly.add_assoc(v, "ab", 96 - 24);
  poly.assoc(v, "", 4);
  poly.assoc(v, "abab", 576);
  assert(u == v);
}
