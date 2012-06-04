#include <vcsn/weights/z.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/weights/poly.hh>
#include <cassert>

int main()
{
  struct context_t
  {
    using genset_t = vcsn::set_alphabet<vcsn::char_letters>;
    genset_t gs_ = { 'a', 'b', 'c', 'd' };
    using weightset_t = vcsn::z;
    weightset_t ws_ = weightset_t{};
  };
  context_t context;
  using poly_t = vcsn::polynomials<context_t>;
  poly_t poly(context);

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
