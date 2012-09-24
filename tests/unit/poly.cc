#include <cassert>
#include <vcsn/ctx/char_z_lal.hh>
#include <vcsn/weights/poly.hh>
#include <tests/unit/test.hh>

static
bool
check_assoc()
{
  bool res = true;
  using context_t = vcsn::ctx::char_z_lal;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  using poly_t = vcsn::polynomialset<context_t>;
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

  return res;
}


static
bool
check_conv()
{
  bool res = true;

  using context_t = vcsn::ctx::char_z_lal;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  using poly_t = vcsn::polynomialset<context_t>;
  poly_t poly{ctx};

#define CHECK(In, Out)                                   \
  ASSERT_EQ(poly.format(poly.conv(In)), Out);

  CHECK("\\e", "\\e");
  CHECK("a", "a");
  CHECK("a+b", "a + b");
  CHECK("a+a+a", "{3}a");
  CHECK("a+b+a", "{2}a + b");
  CHECK("{2}a+{3}b+{5}c+{10}a+{10}c+{10}b+{10}d",
        "{12}a + {13}b + {15}c + {10}d");
  // Check long numbers before smaller ones to exercise some issues
  // when reusing an ostringstream: we might keep previous characters.
  CHECK("{1000}a + {1}a + {0}a",
        "{1001}a");
#undef CHECK
  return res;
}

int main()
{
  size_t errs = 0;
  errs += !check_assoc();
  errs += !check_conv();
  return !!errs;
}
