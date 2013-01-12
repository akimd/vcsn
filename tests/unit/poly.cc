#include <cassert>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/ctx/lal_char_zmin.hh>
#include <vcsn/weights/poly.hh>
#include <tests/unit/test.hh>

template <class T>
static
bool
check_conv_fail(T& poly, const std::string& str)
{
  try
    {
      poly.conv(str);
    }
  catch (std::domain_error&)
    {
      return true;
    }
  return false;
}


static
bool
check_assoc()
{
  bool res = true;
  using context_t = vcsn::ctx::lal_char_z;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  using poly_t = vcsn::polynomialset<context_t>;
  poly_t poly{ctx};

  poly_t::value_t u = poly.unit();
  poly.set_weight(u, "ab", 12);
  ASSERT_EQ(poly.format(u), "\\e + {12}ab");

  u = poly.add(u, u);
  ASSERT_EQ(poly.format(u), "{2}\\e + {24}ab");

  assert(!poly.is_zero(u));
  assert(poly.is_zero(poly.zero()));

  poly_t::value_t v;
  poly.set_weight(v, "ab", 24);
  poly.set_weight(v, "", 2);
  assert(u == v);

  u = poly.mul(u, u);
  ASSERT_EQ(poly.format(u), "{4}\\e + {96}ab + {576}abab");

  poly.add_weight(v, "ab", 96 - 24);
  poly.set_weight(v, "", 4);
  poly.set_weight(v, "abab", 576);
  assert(u == v);

  return res;
}


static
bool
check_conv()
{
  bool res = true;

  using context_t = vcsn::ctx::lal_char_z;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  using poly_t = vcsn::polynomialset<context_t>;
  poly_t poly{ctx};

#define CHECK(In, Out)                                   \
  ASSERT_EQ(poly.format(poly.conv(In)), Out);
#define CHECK_FAIL(In)                                   \
  ASSERT_EQ(check_conv_fail(poly, In), true);

  CHECK("\\e", "\\e");
  CHECK("\\z", "\\z");
  CHECK_FAIL("");
  CHECK("a", "a");
  CHECK("a+b", "a + b");
  CHECK("a+a+a", "{3}a");
  CHECK("a+b+a", "{2}a + b");
  CHECK_FAIL("a++a");
  CHECK_FAIL("+a");
  CHECK_FAIL("a+");
  CHECK("{2}a+{3}b+{5}c+{10}a+{10}c+{10}b+{10}d",
        "{12}a + {13}b + {15}c + {10}d");
  CHECK_FAIL("{2}a++{3}a");
  CHECK_FAIL("+{1}a");
  CHECK_FAIL("{2}a+");
  CHECK("{2}+a", "{2}\\e + a");
  CHECK("  {2}  ", "{2}\\e");
  CHECK("a+{1}a+{-2}a", "\\z");
  // Check long numbers before smaller ones to exercise some issues
  // when reusing an ostringstream: we might keep previous characters.
  CHECK("{1000}a + {1}a + {0}a",
        "{1001}a");
#undef CHECK_FAIL
#undef CHECK
  return res;
}


template <class T>
static
bool
check_star_fail(T& poly, const std::string& str)
{
  try
    {
      poly.star(poly.conv(str));
    }
  catch (std::domain_error&)
    {
      return true;
    }
  return false;
}


static
bool
check_star()
{
  bool res = true;

  using context_t = vcsn::ctx::lal_char_zmin;
  context_t ctx {{'a', 'b'}};
  using poly_t = vcsn::polynomialset<context_t>;
  poly_t poly{ctx};

  ASSERT_EQ(poly.format(poly.star(poly.conv("{123}\\e"))), "{0}\\e");
  ASSERT_EQ(poly.format(poly.star(poly.conv("{oo}\\e"))), "{0}\\e");
  ASSERT_EQ(poly.format(poly.star(poly.conv("{123}\\e+{oo}\\e"))), "{0}\\e");
  ASSERT_EQ(poly.format(poly.star(poly.conv("\\z"))), "{0}\\e");
  ASSERT_EQ(check_star_fail(poly, "{-1}\\e"), true);
  ASSERT_EQ(check_star_fail(poly, "{123}a"), true);
  ASSERT_EQ(check_star_fail(poly, "{123}\\e+{12}a"), true);
  ASSERT_EQ(poly.format(poly.star(poly.conv("{12}\\e+{oo}a"))), "{0}\\e");
  ASSERT_EQ(check_star_fail(poly, "{123}\\e+{oo}a+{3}a"), true);
  return res;
}

int main()
{
  size_t errs = 0;
  errs += !check_assoc();
  errs += !check_conv();
  errs += !check_star();
  return !!errs;
}
