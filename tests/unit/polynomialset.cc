#include <cassert>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/ctx/lal_char_zmin.hh>
#include <vcsn/weights/polynomialset.hh>
#include <tests/unit/test.hh>

template <class T>
static
bool
check_conv_fail(T& ps, const std::string& str)
{
  try
    {
      ps.conv(str);
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
  using ps_t = vcsn::polynomialset<context_t>;
  ps_t ps{ctx};

  ps_t::value_t u = ps.unit();
  ps.set_weight(u, "ab", 12);
  ASSERT_EQ(ps.format(u), "\\e + {12}ab");

  u = ps.add(u, u);
  ASSERT_EQ(ps.format(u), "{2}\\e + {24}ab");

  assert(!ps.is_zero(u));
  assert(ps.is_zero(ps.zero()));

  ps_t::value_t v;
  ps.set_weight(v, "ab", 24);
  ps.set_weight(v, "", 2);
  assert(u == v);

  u = ps.mul(u, u);
  ASSERT_EQ(ps.format(u), "{4}\\e + {96}ab + {576}abab");

  ps.add_weight(v, "ab", 96 - 24);
  ps.set_weight(v, "", 4);
  ps.set_weight(v, "abab", 576);
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
  using ps_t = vcsn::polynomialset<context_t>;
  ps_t ps{ctx};

#define CHECK(In, Out)                                   \
  ASSERT_EQ(ps.format(ps.conv(In)), Out);
#define CHECK_FAIL(In)                                   \
  ASSERT_EQ(check_conv_fail(ps, In), true);

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
check_star_fail(T& ps, const std::string& str)
{
  try
    {
      ps.star(ps.conv(str));
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
  using ps_t = vcsn::polynomialset<context_t>;
  ps_t ps{ctx};

  ASSERT_EQ(ps.format(ps.star(ps.conv("{123}\\e"))), "{0}\\e");
  ASSERT_EQ(ps.format(ps.star(ps.conv("{oo}\\e"))), "{0}\\e");
  ASSERT_EQ(ps.format(ps.star(ps.conv("{123}\\e+{oo}\\e"))), "{0}\\e");
  ASSERT_EQ(ps.format(ps.star(ps.conv("\\z"))), "{0}\\e");
  ASSERT_EQ(check_star_fail(ps, "{-1}\\e"), true);
  ASSERT_EQ(check_star_fail(ps, "{123}a"), true);
  ASSERT_EQ(check_star_fail(ps, "{123}\\e+{12}a"), true);
  ASSERT_EQ(ps.format(ps.star(ps.conv("{12}\\e+{oo}a"))), "{0}\\e");
  ASSERT_EQ(check_star_fail(ps, "{123}\\e+{oo}a+{3}a"), true);
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
