#include <vcsn/weights/z.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/weights/poly.hh>
#include <cassert>

int main()
{
  typedef vcsn::set_alphabet<vcsn::char_letters> alpha_t;

  alpha_t alpha{ 'a', 'b', 'c', 'd' };
  struct context_t
  {
    using genset_t = alpha_t;
    using weightset_t = vcsn::z;
  };

  using poly_t = vcsn::polynomials<context_t>;
  poly_t poly(alpha);

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
